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
	mcp23s08_handle_t mcp_handle;
	mcp23s08_init(&mcp_handle, &(sg_ctx->mcp_cfg));
	sg_ctx->mcp_handle = mcp_handle;
	mcp23s08_write(sg_ctx->mcp_handle, sg_ctx->hw_adr, IOCON, (1<<HAEN));

	// ------------------------------------------------------------
	// interrupt GPIO configuration move to mcp c file
	// ------------------------------------------------------------

	// if (sg_ctx->((mcp23s08_context_t)mcp_handle)->cfg.intr_io >= 0)
	// {
	// 	gpio_config_t gpio_intr_cfg = {
	// 		.mode = GPIO_MODE_INPUT,
	// 		.intr_type = GPIO_INTR_POSEDGE,
	// 		.pin_bit_mask = 1ULL << sg_ctx->mcp_handle->cfg.intr_io,
	// 		.pull_down_en = GPIO_PULLDOWN_DISABLE,
	// 		.pull_up_en = GPIO_PULLUP_DISABLE,
	// 	};
	// 	gpio_config(&gpio_intr_cfg);
	// 	gpio_set_intr_type(sg_ctx->mcp_handle->cfg.intr_io, GPIO_INTR_POSEDGE);
	// 	mcp_evt_queue = xQueueCreate(10, sizeof(uint32_t));
	// 	xTaskCreate(mcp_task, "mcp_input_interrupt", 2048, NULL, 10, NULL);
	// 	gpio_install_isr_service(0);
	// 	gpio_isr_handler_add(sg_ctx->mcp_handle->cfg.intr_io, mcp_isr_handler, (void *)sg_ctx->mcp_handle->cfg.intr_io);
	// }

	// ------------------------------------------------------------
	// Sevensegment MUX configuration
	// ------------------------------------------------------------
	gpio_config_t mux_cfg = {
		.pin_bit_mask = MUX_MASK,
		.mode = GPIO_MODE_OUTPUT,
	};
	gpio_config(&mux_cfg);
}
// void s_seg_shift(s_seg_context_t *sg_ctx, int8_t dir);
// void s_seg_write(s_seg_context_t *sg_ctx, char *buf);
void s_seg_write_single(s_seg_context_t *sg_ctx, uint8_t s_seg_channel, char data)
{
	gpio_set_level(s_seg_channel, 1);
	mcp23s08_write(sg_ctx->mcp_handle, sg_ctx->hw_adr, IODIR, 0x00);
	mcp23s08_write(sg_ctx->mcp_handle, sg_ctx->hw_adr, GPIO_R, data);
}
