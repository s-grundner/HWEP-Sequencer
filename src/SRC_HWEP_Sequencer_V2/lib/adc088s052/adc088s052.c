#include "adc088s052.h"
#define CS_ADC0880S052 (GPIO_NUM_15)
// ------------------------------------------------------------
// (Public) Functions
// ------------------------------------------------------------

static void cs_high(spi_transaction_t *t)
{
	gpio_set_level(CS_ADC0880S052, 1);
}

static void cs_low(spi_transaction_t *t)
{
	gpio_set_level(CS_ADC0880S052, 0);
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
		.mode = 0,
		.flags = SPI_DEVICE_HALFDUPLEX,
		.spics_io_num = CS_ADC0880S052,
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
	free(ctx);
	return ESP_OK;
}
esp_err_t adc088s052_get_raw(adc088s052_context_t *ctx, adc088s052_channel_t ch, uint16_t *data)
{
	
	esp_err_t err;
	err = spi_device_acquire_bus(ctx->spi, portMAX_DELAY);
	spi_transaction_t t = {
		.cmd = 0,
		.addr = (ch+1) << 3,
		.rxlength = 16,
		.flags = SPI_TRANS_USE_RXDATA,
		.user = ctx,
	};

	err = spi_device_polling_transmit(ctx->spi, &t);
	if (err != ESP_OK)
		return err;
	// TODO: SPI_SWAP_DATA_RX
	*data = t.rx_data[0];
	spi_device_release_bus(ctx->spi);
	return ESP_OK;
}