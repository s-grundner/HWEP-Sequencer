#include "sequencer.h"

static QueueHandle_t mcp_evt_queue = NULL;

static void IRAM_ATTR mcp_isr_handler(void *arg)
{
	mcp23s08_handle_t mcp_handle = (mcp23s08_handle_t)arg;
	xQueueSendFromISR(mcp_evt_queue, &mcp_handle, NULL);
}

static void mcp_task(void *arg)
{
	mcp23s08_handle_t mcp_handle;
	for (;;)
	{
		if (xQueueReceive(mcp_evt_queue, &mcp_handle, portMAX_DELAY))
		{
			uint8_t data = 0;
			esp_err_t err = mcp23s08_read(mcp_handle, HW_ADR_0, GPIO_R, &data);
			ESP_ERROR_CHECK(err);
			printf("%x\n", data);
		}
	}
}

void s_seg_init(s_seg_context_t *sg_ctx)
{
	if (sg_ctx->mcp_ctx->cfg.intr_io >= 0)
	{
		gpio_config_t gpio_intr_cfg = {
			.mode = GPIO_MODE_INPUT,
			.intr_type = GPIO_INTR_POSEDGE,
			.pin_bit_mask = 1 << sg_ctx->mcp_ctx->cfg.intr_io,
			.pull_down_en = GPIO_PULLDOWN_DISABLE,
			.pull_up_en = GPIO_PULLUP_DISABLE,
		};
		gpio_config(&gpio_intr_cfg);
		gpio_set_intr_type(GPIO_NUM_32, GPIO_INTR_POSEDGE);
	}
	mcp_evt_queue = xQueueCreate(10, sizeof(uint32_t));
	xTaskCreate(mcp_task, "mcp_input_interrupt", 2048, NULL, 10, NULL);
	gpio_install_isr_service(0);
	gpio_isr_handler_add(sg_ctx->mcp_ctx->cfg.intr_io, mcp_isr_handler, (void *)sg_ctx->mcp_ctx->cfg.intr_io);
}
void s_seg_shift(s_seg_context_t *sg_ctx, int8_t dir);
void s_seg_write(char *buf);
