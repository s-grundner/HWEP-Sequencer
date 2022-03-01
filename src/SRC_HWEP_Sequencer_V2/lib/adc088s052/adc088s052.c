#include "adc088s052.h"

// ------------------------------------------------------------
// (Public) Functions
// ------------------------------------------------------------

static void cs_high(spi_transaction_t *t)
{
	gpio_set_level(((adc088s052_context_t*)t->user)->cfg.cs_io, 1);
}

static void cs_low(spi_transaction_t *t)
{
	gpio_set_level(((adc088s052_context_t*)t->user)->cfg.cs_io, 0);
}

// ------------------------------------------------------------
// (Public) Functions
// ------------------------------------------------------------

esp_err_t adc088s052_init(adc088s052_context_t **out_ctx, const adc088s052_config_t *cfg)
{
	esp_err_t err = ESP_OK;

	adc088s052_context_t *ctx = (adc088s052_context_t *)malloc(sizeof(adc088s052_context_t));
	if (!ctx)
		return ESP_ERR_NO_MEM;

	*ctx = (adc088s052_context_t){
		.cfg = *cfg,
	};

	spi_device_interface_config_t devcfg = {
		.command_bits = 0,
		.address_bits = 8,
		.clock_speed_hz = F_SCK_ADC,
		.mode = 0, // SPI mode 0
		.spics_io_num = ctx->cfg.cs_io,
		.queue_size = 1,
		.pre_cb = cs_high,
		.post_cb = cs_low,
	};

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

	printf("%d\n", ((adc088s052_handle_t)*out_ctx)->cfg.cs_io);

	free(ctx);
	return ESP_OK;
}
esp_err_t adc088s052_get_raw(adc088s052_context_t *ctx, adc088s052_channel_t ch, uint8_t *data)
{
	esp_err_t err;

	uint16_t data_local = malloc(sizeof(uint16_t));
	spi_transaction_t t = {
		.cmd = 0,
		.addr = ch << 3,
		.rxlength = 16,
		.flags = SPI_TRANS_USE_RXDATA,
		.user = ctx,
	};

	err = spi_device_polling_transmit(ctx->spi, &t);
	if (err != ESP_OK)
		return err;

	data_local = t.rx_data[0];
	*data = (uint8_t)data_local << 4;
	free(data_local);
	return ESP_OK;
}
esp_err_t adc088s052_get(adc088s052_context_t *ctx, uint8_t *data[8])
{
	esp_err_t err;

	spi_transaction_t t = {
		.cmd = 0,
		.rxlength = 16,
		.flags = SPI_TRANS_USE_RXDATA,
		.user = ctx,
	};

	for (int i = 0; i < sizeof(adc088s052_channel_t) / sizeof(ADC088S052_CH_0); i++)
	{
		t.addr = i << 3;
		err = spi_device_polling_transmit(ctx->spi, &t);
		if (err != ESP_OK)
			return err;
		*data[i] = t.rx_data[0];
	}
	return ESP_OK;
}