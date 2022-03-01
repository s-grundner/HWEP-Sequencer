#include "encoder.h"

// ------------------------------------------------------------
// private functions
// ------------------------------------------------------------

static void encoder_update(encoder_states_t *arg)
{
	uint8_t s = arg->state & 3;
	if (gpio_get_level(arg->pin_a))
		s |= 4;
	if (gpio_get_level(arg->pin_b))
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

// ------------------------------------------------------------
// ISR for pin A
// ------------------------------------------------------------

static QueueHandle_t ec_evt_queue = NULL;
static void IRAM_ATTR ec_isr_handler(void *arg)
{
	encoder_states_t *states = (encoder_states_t *)arg;
	xQueueSendFromISR(a_evt_queue, &states, NULL);
}
static void ec_task(void *arg)
{
	encoder_states_t *states;
	for (;;)
	{
		if (xQueueReceive(ec_evt_queue, &states, portMAX_DELAY))
		{
			encoder_update(states);
		}
	}
}

// ------------------------------------------------------------
// ISR for pin SW
// ------------------------------------------------------------

static QueueHandle_t sw_evt_queue = NULL;
static void IRAM_ATTR sw_isr_handler(void *arg)
{
	encoder_states_t *states = (encoder_states_t *)arg;
	xQueueSendFromISR(sw_evt_queue, &states, NULL);
}
static void sw_task(void *arg)
{
	encoder_states_t *states;
	for (;;)
	{
		if (xQueueReceive(sw_evt_queue, &states, portMAX_DELAY))
		{
			states->sw_callback(states);
		}
	}
}

// ------------------------------------------------------------
// public functions
// ------------------------------------------------------------

void encoder_init(encoder_states_t *ec)
{
	gpio_config_t ec_ab = {
		.intr_type = GPIO_PIN_INTR_ANYEDGE,
		.mode = GPIO_MODE_INPUT,
		.pull_down_en = GPIO_PULLDOWN_DISABLE,
		.pull_up_en = GPIO_PULLUP_ENABLE,
		.pin_bit_mask = (1ULL << ec->pin_a) | (1ULL << ec->pin_b),
	};
	gpio_config_t ec_sw = {
		.intr_type = GPIO_PIN_INTR_NEGEDGE,
		.mode = GPIO_MODE_INPUT,
		.pull_down_en = GPIO_PULLDOWN_DISABLE,
		.pull_up_en = GPIO_PULLUP_ENABLE,
		.pin_bit_mask = (1ULL << ec->pin_sw),
	};
	gpio_config(&ec_ab);
	gpio_config(&ec_sw);

	gpio_set_intr_type(ec->pin_a, GPIO_INTR_ANYEDGE);
	gpio_set_intr_type(ec->pin_b, GPIO_INTR_ANYEDGE);
	gpio_set_intr_type(ec->pin_sw, GPIO_INTR_NEGEDGE);

	ec_evt_queue = xQueueCreate(10, sizeof(uint32_t));
	xTaskCreate(ec_task, "encoder_interrupt", 2048, NULL, 10, NULL);


	sw_evt_queue = xQueueCreate(10, sizeof(uint32_t));
	xTaskCreate(sw_task, "sw_input_interrupt", 2048, NULL, 10, NULL);

	gpio_install_isr_service(0);

	gpio_isr_handler_add(ec->pin_a, ec_isr_handler, (void *)ec);
	gpio_isr_handler_add(ec->pin_b, ec_isr_handler, (void *)ec);
	gpio_isr_handler_add(ec->pin_sw, sw_isr_handler, (void *)ec);
}
