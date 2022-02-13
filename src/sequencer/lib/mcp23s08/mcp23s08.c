#include "mcp23s08.h"

// static const char TAG[] = "mcp23s08";

static void cs_high(spi_transaction_t *t)
{
	gpio_set_level(((mcp23s08_context_t *)t->user)->cfg.cs_io, 1);
}

static void cs_low(spi_transaction_t *t)
{
	gpio_set_level(((mcp23s08_context_t *)t->user)->cfg.cs_io, 0);
}

esp_err_t mcp23s08_init(mcp23s08_context_t **out_ctx, const mcp23s08_config_t *cfg)
{
	esp_err_t err;

	mcp23s08_context_t *ctx = (mcp23s08_context_t *)malloc(sizeof(mcp23s08_context_t));

	spi_device_interface_config_t devcfg = {
		.clock_speed_hz = F_SCK,
		.mode = 0,
		.spics_io_num = cfg->cs_io,
		.queue_size = 3,
		.command_bits = 8,
		.address_bits = 8,
		.dummy_bits = 0,
		.pre_cb = cs_high,
		.post_cb = cs_low,
	};
	err = spi_bus_add_device(VSPI_HOST, &devcfg, &ctx->spi);
	return err;
}

// esp_err_t mcp23s08_read(spi_device_handle_t spi, mcp23s08_hw_adr hw_adr, mcp23s08_reg_adr reg_adr, uint8_t *data)
// {
// 	esp_err_t err;
// 	uint8_t opcode = 0x41 | (hw_adr << 1);
// 	return err;
// }

esp_err_t mcp23s08_write(mcp23s08_context_t *ctx, mcp23s08_hw_adr hw_adr, mcp23s08_reg_adr reg_adr, uint8_t data)
{
	esp_err_t err;
	err = spi_device_acquire_bus(ctx->spi, portMAX_DELAY);
	if (err != ESP_OK)
		return err;

	spi_transaction_t t = {
		.cmd = 0x40 | (hw_adr << 1),
		.addr = reg_adr,
		.length = 8,
		.tx_data = {data},
		.user = ctx,
	};

	err = spi_device_polling_transmit(ctx->spi, &t);

	// wait for transaction complete function?

	spi_device_release_bus(ctx->spi);
	return err;
}