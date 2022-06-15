#include "mcp23s08.h"
#include "esp_log.h"
#define CS_MCP23S08 (GPIO_NUM_5)
#define MCP_BUSY_TIMEOUT_MS (5)

struct mcp23s08_context_t
{
	mcp23s08_config_t cfg;
	spi_device_handle_t spi;
	SemaphoreHandle_t ready_sem;
};

typedef struct mcp23s08_context_t mcp23s08_context_t;

static const char *TAG = "mcp23s08";

// ------------------------------------------------------------
// (Private) Functions
// ------------------------------------------------------------

static void cs_high(spi_transaction_t *t)
{
	gpio_set_level(CS_MCP23S08, 1);
}

static void cs_low(spi_transaction_t *t)
{
	gpio_set_level(CS_MCP23S08, 0);
}

// ------------------------------------------------------------
// ISR
// ------------------------------------------------------------

static QueueHandle_t mcp_evt_queue = NULL;
static void IRAM_ATTR mcp_isr_handler(void *arg)
{
	mcp23s08_context_t *ctx = (mcp23s08_context_t *)arg;
	xQueueSendFromISR(mcp_evt_queue, &ctx, NULL);
}
static void mcp_intr_task(void *arg)
{
	mcp23s08_context_t *ctx;
	for (;;)
	{
		if (xQueueReceive(mcp_evt_queue, &ctx, portMAX_DELAY))
		{
			xSemaphoreTake(ctx->ready_sem, 2);
			ctx->cfg.mcp_callback(ctx->cfg.mcp_intr_args);
			
		}
	}
}

// ------------------------------------------------------------
// (Public) Functions
// ------------------------------------------------------------

esp_err_t mcp23s08_init(mcp23s08_context_t **out_ctx, const mcp23s08_config_t *cfg)
{
	esp_err_t err = ESP_OK;

	mcp23s08_context_t *ctx = (mcp23s08_context_t *)malloc(sizeof(mcp23s08_context_t));
	if (!ctx)
		return ESP_ERR_NO_MEM;

	*ctx = (mcp23s08_context_t){
		.cfg = *cfg,
	};

	spi_device_interface_config_t devcfg = {
		.command_bits = 8,
		.address_bits = 8,
		.clock_speed_hz = F_SCK_MCP,
		.mode = 0,
		.flags = SPI_DEVICE_HALFDUPLEX,
		.spics_io_num = ctx->cfg.cs_io,
		.queue_size = 1,
		.pre_cb = cs_high,
		.post_cb = cs_low,
	};

	if (ctx->cfg.intr_io >= 0)
	{
		ctx->ready_sem = xSemaphoreCreateBinary();
		if (ctx->ready_sem == NULL)
		{
			err = ESP_ERR_NO_MEM;
			goto cleanup;
		}
		gpio_config_t intr_cfg = {
			.intr_type = GPIO_INTR_POSEDGE,
			.pull_down_en = 1,
			.pull_up_en = 0,
			.mode = GPIO_MODE_INPUT,
			.pin_bit_mask = 1ULL << ctx->cfg.intr_io,
		};
		gpio_config(&intr_cfg);
		mcp_evt_queue = xQueueCreate(10, sizeof(uint32_t));
		xTaskCreate(mcp_intr_task, "mcp23s08_interrupt", 2048, NULL, 10, NULL);

		// gpio_install_isr_service(ESP_INTR_FLAG_EDGE);

		gpio_isr_handler_add(ctx->cfg.intr_io, mcp_isr_handler, (void *)ctx);
	}

	err = spi_bus_add_device(ctx->cfg.host, &devcfg, &ctx->spi);
	ESP_ERROR_CHECK(err);
	if (err != ESP_OK)
	{
		ESP_LOGE(TAG, "Failed to add device to spi bus");
		goto cleanup;
	}

	*out_ctx = ctx;
	return ESP_OK;

cleanup:
	if (ctx->spi)
	{
		spi_bus_remove_device(ctx->spi);
		ctx->spi = NULL;
	}
	if (ctx->ready_sem)
	{
		vSemaphoreDelete(ctx->ready_sem);
		ctx->ready_sem = NULL;
	}
	free(ctx);
	return err;
}

esp_err_t mcp23s08_read(mcp23s08_context_t *ctx, mcp23s08_hw_adr hw_adr, mcp23s08_reg_adr reg_adr, uint8_t *data)
{
	esp_err_t err = ESP_OK;
	err = spi_device_acquire_bus(ctx->spi, portMAX_DELAY);
	if (err != ESP_OK)
		return err;

	uint8_t opcode = 0x41 | (hw_adr << 1);

	spi_transaction_t t = {
		.cmd = opcode,
		.addr = (uint8_t)reg_adr,
		.rxlength = 8,
		.flags = SPI_TRANS_USE_RXDATA,
		.user = ctx,
	};
	err = spi_device_polling_transmit(ctx->spi, &t);

	if ((err == ESP_OK) && (ctx->cfg.intr_io >= 0))
		xSemaphoreGive(ctx->ready_sem);

	spi_device_release_bus(ctx->spi);

	*data = t.rx_data[0];
	return err;
}

esp_err_t mcp23s08_write(mcp23s08_context_t *ctx, mcp23s08_hw_adr hw_adr, mcp23s08_reg_adr reg_adr, const uint8_t data)
{
	esp_err_t err = ESP_OK;
	err = spi_device_acquire_bus(ctx->spi, portMAX_DELAY);
	if (err != ESP_OK)
		return err;

	uint8_t opcode = 0x40 | (hw_adr << 1);
	spi_transaction_t t = {
		.cmd = opcode,
		.addr = (uint8_t)reg_adr,
		.length = 8,
		.flags = SPI_TRANS_USE_TXDATA,
		.tx_data = {data},
		.user = ctx,
	};

	err = spi_device_polling_transmit(ctx->spi, &t);

	if ((err == ESP_OK) && (ctx->cfg.intr_io >= 0))
		xSemaphoreGive(ctx->ready_sem);

		spi_device_release_bus(ctx->spi);
	return err;
}

esp_err_t mcp23s08_dump_intr(mcp23s08_context_t *ctx, mcp23s08_hw_adr hw_adr)
{
	esp_err_t err = ESP_OK;
	uint8_t opcode = 0x41 | (hw_adr << 1);

	spi_transaction_t t = {
		.cmd = opcode,
		.addr = (uint8_t)INTCAP,
		.rxlength = 8,
		.flags = SPI_TRANS_USE_RXDATA,
		.user = ctx,
	};
	err = spi_device_polling_transmit(ctx->spi, &t);
	ESP_ERROR_CHECK(err);
	return err;
}

void mcp23s08_take_sem(mcp23s08_context_t *ctx)
{
	xSemaphoreTake(ctx->ready_sem, (TickType_t)2);
}
void mcp23s08_give_sem(mcp23s08_context_t *ctx)
{
	xSemaphoreGive(ctx->ready_sem);
}