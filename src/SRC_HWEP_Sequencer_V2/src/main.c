#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2s.h"
#include "driver/gpio.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/rmt.h"
#include "esp_timer.h"
#include <math.h>

#include "sequencer.h"

static const char *TAG = "sequencer";

static void timer_cb(void *args)
{
	sequencer_config_t *ctx = (sequencer_config_t*)args;
	stp16cp05_write(ctx->stp_handle, ctx->cur_stp_high, (uint8_t)pow(2, ctx->channel));
	adc088s052_get_raw(ctx->adc_handle, ctx->channel, &ctx->cur_adc_data[ctx->channel]);
	ctx->channel = (ctx->channel + 1) % ADC0880S052_CHANNEL_MAX;
}

void app_main(void)
{
	sequencer_handle_t sqc_handle;
	sequencer_init(&sqc_handle);

	int prev_pos = encoder_read(sqc_handle->encoder_handle) + 1;

	esp_timer_handle_t bpm_timer;
	esp_timer_create_args_t bpm_timer_cfg = {
		.name = "bpm",
		.callback = &timer_cb,
		.arg = &sqc_handle,
	};

	ESP_ERROR_CHECK(esp_timer_create(&bpm_timer_cfg, &bpm_timer));
	ESP_ERROR_CHECK(esp_timer_start_periodic(bpm_timer, BPM_TO_US(sqc_handle->cur_bpm)));

	while (1)
	{
		sqc_handle->encoder_positions[sqc_handle->cur_appmode] = encoder_read(sqc_handle->encoder_handle);
		
		switch (sqc_handle->cur_appmode)
		{
		case APP_MODE_BPM:
			
			break;
		case APP_MODE_KEY:

			break;
		case APP_MODE_ENR:

			break;
		case APP_MODE_TSP:

			break;
		default:
			break;
		}

		send_audio_stereo(&sqc_handle->osc);
		vTaskDelay(100 / portTICK_PERIOD_MS);
	}
}


