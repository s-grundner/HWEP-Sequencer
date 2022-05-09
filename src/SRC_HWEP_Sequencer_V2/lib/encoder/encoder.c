#include "encoder.h"

// ------------------------------------------------------------
// private functions
// ------------------------------------------------------------

static void encoder_update(encoder_context_t *arg)
{
	arg->cfg.rot_callback(arg->cfg.rot_args ? arg->cfg.rot_args : arg);
	uint8_t s = arg->state & 3;
	if (gpio_get_level(arg->cfg.pin_a))
		s |= 4;
	if (gpio_get_level(arg->cfg.pin_b))
		s |= 8;
	switch (s)
	{
	case 0:
	case 5:
	case 10:
	case 15:
		break;
	case 1:
	case 7:
	case 8:
	case 14:
		arg->position--;
		break;
	case 2:
	case 4:
	case 11:
	case 13:
		arg->position++;
		break;
	case 3:
	case 12:
		arg->position -= 2;
		break;
	default:
		arg->position += 2;
		break;
	}
	arg->state = (s >> 2);
}
static void encoder_switch_update(encoder_context_t *arg)
{
	arg->cfg.sw_callback(arg->cfg.sw_args ? arg->cfg.sw_args : arg);
	arg->sw_state = (arg->sw_state >= arg->cfg.sw_max) ? 0 : arg->sw_state + 1;
}

// ------------------------------------------------------------
// ISR
// ------------------------------------------------------------

static QueueHandle_t ec_evt_queue = NULL;
static void IRAM_ATTR ec_isr_handler(void *arg)
{
	encoder_context_t *context = (encoder_context_t *)arg;
	xQueueSendFromISR(ec_evt_queue, &context, NULL);
}
static void ec_task(void *arg)
{
	encoder_context_t *context;
	for (;;)
	{
		if (xQueueReceive(ec_evt_queue, &context, portMAX_DELAY))
		{
			encoder_update(context);
		}
	}
}

static QueueHandle_t sw_evt_queue = NULL;
static void IRAM_ATTR sw_isr_handler(void *arg)
{
	encoder_context_t *context = (encoder_context_t *)arg;
	xQueueSendFromISR(sw_evt_queue, &context, NULL);
}
static void sw_task(void *arg)
{
	encoder_context_t *context;
	for (;;)
	{
		if (xQueueReceive(sw_evt_queue, &context, portMAX_DELAY))
		{
			encoder_switch_update(context);
		}
	}
}

static void init_intr(encoder_context_t *ec)
{
	gpio_config_t ec_ab = {
		.intr_type = GPIO_PIN_INTR_ANYEDGE,
		.mode = GPIO_MODE_INPUT,
		.pull_down_en = GPIO_PULLDOWN_DISABLE,
		.pull_up_en = GPIO_PULLUP_ENABLE,
		.pin_bit_mask = (1ULL << ec->cfg.pin_a) | (1ULL << ec->cfg.pin_b),
	};
	gpio_config_t ec_sw = {
		.intr_type = GPIO_PIN_INTR_NEGEDGE,
		.mode = GPIO_MODE_INPUT,
		.pull_down_en = GPIO_PULLDOWN_DISABLE,
		.pull_up_en = GPIO_PULLUP_ENABLE,
		.pin_bit_mask = (1ULL << ec->cfg.pin_sw),
	};
	gpio_config(&ec_ab);
	gpio_config(&ec_sw);

	gpio_set_intr_type(ec->cfg.pin_a, GPIO_INTR_ANYEDGE);
	gpio_set_intr_type(ec->cfg.pin_b, GPIO_INTR_ANYEDGE);
	gpio_set_intr_type(ec->cfg.pin_sw, GPIO_INTR_NEGEDGE);

	ec_evt_queue = xQueueCreate(10, sizeof(uint32_t));
	xTaskCreate(ec_task, "encoder_interrupt", 2048, NULL, 10, NULL);

	sw_evt_queue = xQueueCreate(10, sizeof(uint32_t));
	xTaskCreate(sw_task, "sw_input_interrupt", 2048, NULL, 10, NULL);

	gpio_install_isr_service(0);

	gpio_isr_handler_add(ec->cfg.pin_a, ec_isr_handler, (void *)ec);
	gpio_isr_handler_add(ec->cfg.pin_b, ec_isr_handler, (void *)ec);
	gpio_isr_handler_add(ec->cfg.pin_sw, sw_isr_handler, (void *)ec);
}

// ------------------------------------------------------------
// public functions
// ------------------------------------------------------------

esp_err_t encoder_init(encoder_context_t **ec_out, encoder_config_t *ec_cfg)
{
	encoder_context_t *ec = (encoder_context_t *)malloc(sizeof(encoder_context_t));
	if (!ec)
		return ESP_ERR_NO_MEM;

	*ec = (encoder_context_t){
		.cfg = *ec_cfg,
		.sw_state = 0,
	};

	*ec_out = ec;
	init_intr(*ec_out);
	return ESP_OK;
}
int32_t encoder_read(encoder_context_t *arg)
{
	gpio_intr_disable(arg->cfg.pin_a);
	gpio_intr_disable(arg->cfg.pin_b);
	int32_t ret = arg->position;
	gpio_intr_enable(arg->cfg.pin_a);
	gpio_intr_enable(arg->cfg.pin_b);
	return ret;
}
void encoder_write(encoder_context_t *arg, int32_t position)
{
	gpio_intr_disable(arg->cfg.pin_a);
	gpio_intr_disable(arg->cfg.pin_b);
	arg->position = position;
	gpio_intr_enable(arg->cfg.pin_a);
	gpio_intr_enable(arg->cfg.pin_b);
}
uint8_t encoder_read_sw(encoder_context_t *arg)
{
	gpio_intr_disable(arg->cfg.pin_sw);
	uint8_t ret = arg->sw_state;
	gpio_intr_enable(arg->cfg.pin_sw);
	return ret;
}