/**
 * @file 	main.c
 * @author	@h-ihninger
 * @author 	@s-grundner
 * @brief 	main file of the HWEP Sequencer Project
 * @version 2
 * @date 2022-05-05
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "sequencer.h"
#include "esp_log.h"

#define DEBUG

static const char *TAG = "sequencer_main";
static const char *INTR_TAG = "intrs";

static void timer_cb(void *args)
{
	ESP_LOGD(INTR_TAG, "BPM Timer Interrupt");

	sequencer_config_t *ctx = (sequencer_config_t *)args;

	// fix adc data arrangement
	ESP_ERROR_CHECK(adc088s052_get_raw(ctx->adc_handle, ctx->channel, &(ctx->cur_adc_data[ctx->channel])));
	uint16_t data = 0;

	ESP_LOGD(TAG, "channel %d pitch: %d", ctx->channel, ctx->cur_adc_data[ctx->channel]);

	ESP_LOGD(INTR_TAG, "%d", ctx->channel);
	ESP_LOGD(INTR_TAG, "%d", ctx->reset_at_n);

	ctx->channel = (ctx->channel + 1) % ctx->reset_at_n;
}

typedef struct {
	uint16_t bpm;
	uint32_t ec_changed[ADC0880S052_CHANNEL_MAX];

}data_changed;


void app_main(void)
{
	sequencer_handle_t sqc_handle;
	ESP_ERROR_CHECK(sequencer_init(&sqc_handle));

	data_changed dch;

	esp_timer_handle_t bpm_timer;
	esp_timer_create_args_t bpm_timer_cfg = {
		.name = "bpm",
		.callback = &timer_cb,
		.arg = sqc_handle,
	};

	ESP_ERROR_CHECK(esp_timer_create(&bpm_timer_cfg, &bpm_timer));
	ESP_ERROR_CHECK(esp_timer_start_periodic(bpm_timer, bpm_to_us(sqc_handle->cur_bpm)));

	for (int i = 0; i < ADC0880S052_CHANNEL_MAX; i++)
		dch.ec_changed[i] = encoder_read(sqc_handle->encoder_handle) + 1;

	while (1)
	{
		sqc_handle->encoder_positions[sqc_handle->cur_appmode] = encoder_read(sqc_handle->encoder_handle);

		ESP_LOGD(TAG, "ec_pos = %d", encoder_read(sqc_handle->encoder_handle));
		ESP_LOGD(TAG, "bpm = %d", 0x3938700 / sqc_handle->cur_bpm);

		switch (sqc_handle->cur_appmode)
		{
		case APP_MODE_BPM:
			ESP_ERROR_CHECK(stp_index(sqc_handle));
			sqc_handle->cur_bpm = START_BPM + sqc_handle->encoder_positions[sqc_handle->cur_appmode];
			break;
		case APP_MODE_KEY:
			ESP_ERROR_CHECK(stp_index(sqc_handle));
			break;
		case APP_MODE_ENR:
			ESP_ERROR_CHECK(stp_cursor(sqc_handle));
			break;
		case APP_MODE_TSP:
			ESP_ERROR_CHECK(stp_index(sqc_handle));
			break;
		default:
			break;
		}
		
		sqc_handle->osc.pitch = adc_to_pitch(sqc_handle->cur_adc_data[sqc_handle->channel], sqc_handle->osc.oct_offset);
		send_audio_stereo(&sqc_handle->osc);
		vTaskDelay(10 / portTICK_PERIOD_MS);
	}
}
