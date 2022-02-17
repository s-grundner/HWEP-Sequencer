#include "mcp23s08.h"

// ------------------------------------------------------------
// (Private) Functions
// ------------------------------------------------------------

static void cs_high(spi_transaction_t *t)
{
	gpio_set_level(5, 1);
}

static void cs_low(spi_transaction_t *t)
{
	gpio_set_level(5, 0);
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
		.mode = 0, // SPI mode 0
		.spics_io_num = ctx->cfg.cs_io,
		.queue_size = 1,
		.pre_cb = cs_high,
		.post_cb = cs_low,
	};

	if (ctx->cfg.intr_io >= 0)
	{
		gpio_config_t gpio_intr_cfg = {
			.mode = GPIO_MODE_INPUT,
			.intr_type = GPIO_INTR_POSEDGE,
			.pin_bit_mask = 1<<ctx->cfg.intr_io,
			.pull_down_en = GPIO_PULLDOWN_DISABLE,
			.pull_up_en = GPIO_PULLUP_DISABLE,
		};
		gpio_config(&gpio_intr_cfg);
	}

	err = spi_bus_add_device(ctx->cfg.host, &devcfg, &ctx->spi);
	if (err != ESP_OK)
	{
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
		return err;
	}
	*out_ctx = ctx;

	free(ctx);
	return ESP_OK;
}

esp_err_t mcp23s08_read(mcp23s08_context_t *ctx, mcp23s08_hw_adr hw_adr, mcp23s08_reg_adr reg_adr, uint8_t *data)
{
	esp_err_t err;
	err = spi_device_acquire_bus(ctx->spi, portMAX_DELAY);
	if (err != ESP_OK)
		return err;

	uint8_t opcode = 0x41 | (hw_adr << 1);

	spi_transaction_t t = {
		.cmd = opcode,
		.addr = (uint8_t)reg_adr,
		.rxlength = 8,
		.flags = SPI_TRANS_USE_RXDATA,
		.rx_data = {*data},
		.user = ctx,
	};
	err = spi_device_polling_transmit(ctx->spi, &t);
	spi_device_release_bus(ctx->spi);
	return err;
	return 0;
}

esp_err_t mcp23s08_write(mcp23s08_context_t *ctx, mcp23s08_hw_adr hw_adr, mcp23s08_reg_adr reg_adr, const uint8_t data)
{
	esp_err_t err;
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
	spi_device_release_bus(ctx->spi);
	return err;
}