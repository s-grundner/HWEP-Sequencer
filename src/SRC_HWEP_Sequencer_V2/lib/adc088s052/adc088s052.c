#include "adc088s052.h"

// ------------------------------------------------------------
// (Public) Functions
// ------------------------------------------------------------

static void cs_high(spi_transaction_t *t)
{
	gpio_set_level(15, 1);
}

static void cs_low(spi_transaction_t *t)
{
	gpio_set_level(15, 0);
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
esp_err_t adc088s052_get_raw(adc088s052_handle_t handle,adc088s052_channel_t ch, uint8_t *data)
{
	return 0;
}
esp_err_t adc088s052_get(adc088s052_handle_t handle, uint8_t *data[8])
{
	return 0;
}