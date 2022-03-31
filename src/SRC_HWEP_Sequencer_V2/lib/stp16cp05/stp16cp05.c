#include "stp16cp05.h"
#include "esp_log.h"
#define CS_STP16CP05 (GPIO_NUM_21)

struct stp16cp05_context_t
{
	stp16cp05_config_t cfg;
	spi_device_handle_t spi;
	SemaphoreHandle_t ready_sem;
};

typedef struct stp16cp05_context_t stp16cp05_context_t;

static const char *TAG = "stp16cp05";

// ------------------------------------------------------------
// (Private) Functions
// ------------------------------------------------------------

static void cs_high(spi_transaction_t *t)
{
	gpio_set_level(CS_STP16CP05, 1);
}

static void cs_low(spi_transaction_t *t)
{
	gpio_set_level(CS_STP16CP05, 0);
}

// ------------------------------------------------------------
// (Public) Functions
// ------------------------------------------------------------

esp_err_t stp16cp05_init(stp16cp05_context_t **out_ctx, const stp16cp05_config_t *cfg)
{
	esp_err_t err = ESP_OK;

	stp16cp05_context_t *ctx = (stp16cp05_context_t *)malloc(sizeof(stp16cp05_context_t));
	if (!ctx)
		return ESP_ERR_NO_MEM;

	*ctx = (stp16cp05_context_t){
		.cfg = *cfg,
	};

	spi_device_interface_config_t devcfg = {
		.command_bits = 8,
		.address_bits = 8,
		.clock_speed_hz = F_SCK_STP,
		.mode = 0,
		.flags = SPI_DEVICE_HALFDUPLEX | SPI_DEVICE_NO_DUMMY | SPI_DEVICE_POSITIVE_CS,
		.spics_io_num = ctx->cfg.cs_io,
		.queue_size = 1,
		.pre_cb = cs_high,
		.post_cb = cs_low,
	};

	err = spi_bus_add_device(ctx->cfg.host, &devcfg, &ctx->spi);
	if (err != ESP_OK)
	{
		ESP_LOGE(TAG, "Failed to add device to spi bus");
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
	*out_ctx = ctx;
	return ESP_OK;
}

esp_err_t stp16cp05_write(stp16cp05_context_t *ctx, const uint8_t data_top, const uint8_t data_bot)
{
	esp_err_t err = ESP_OK;
	ESP_LOGD(TAG, "Aquiring host %d with CS %d", ctx->cfg.host, ctx->cfg.cs_io);
	err = spi_device_acquire_bus(ctx->spi, portMAX_DELAY);
	if (err != ESP_OK)
		return err;

	// printf("stp data : 0x%04x\n", data);

	spi_transaction_t t = { 
		.cmd = data_top,
		.addr = data_bot,
		.length = 0,
		.user = ctx,
	};

	err = spi_device_polling_transmit(ctx->spi, &t);
	spi_device_release_bus(ctx->spi);
	return err;
}