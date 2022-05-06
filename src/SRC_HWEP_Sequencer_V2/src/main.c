/**
 * @file 	main.c
 * @author	@h-ihninger
 * @author 	@s-grundner
 * @brief 	main file of the HWEP Sequencer Project
 * @version 0.1
 * @date 2022-05-05
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "sequencer.h"

static const char *TAG = "sequencer_main";

static void timer_cb(void *args)
{
	sequencer_config_t *ctx = (sequencer_config_t*)args;
	ctx->channel = (ctx->channel + 1) % ctx->reset_at_n;
}

void app_main(void)
{
	sequencer_handle_t sqc_handle;

	ESP_ERROR_CHECK(sequencer_init(&sqc_handle));

	// int prev_pos = encoder_read(sqc_handle->encoder_handle) + 1;

	esp_timer_handle_t bpm_timer;
	esp_timer_create_args_t bpm_timer_cfg = {
		.name = "bpm",
		.callback = &timer_cb,
		.arg = &sqc_handle,
	};

	ESP_ERROR_CHECK(esp_timer_create(&bpm_timer_cfg, &bpm_timer));
	ESP_ERROR_CHECK(esp_timer_start_periodic(bpm_timer, bpmtous(sqc_handle->cur_bpm)));

	while (1)
	{
		sqc_handle->encoder_positions[sqc_handle->encoder_handle->sw_state] = encoder_read(sqc_handle->encoder_handle);
		ESP_ERROR_CHECK(adc088s052_get_raw(sqc_handle->adc_handle, sqc_handle->channel, &(sqc_handle->cur_adc_data[sqc_handle->channel])));
		
		switch (sqc_handle->encoder_handle->sw_state)
		{
		case APP_MODE_BPM:
			stp_index(sqc_handle);
		case APP_MODE_KEY:
			stp_index(sqc_handle);
			break;
		case APP_MODE_ENR:
			stp_cursor(sqc_handle);
			break;
		case APP_MODE_TSP:
			stp_index(sqc_handle);
			break;
		default:
			break;
		}

		sqc_handle->osc.pitch = adc_to_pitch(sqc_handle->cur_adc_data[sqc_handle->channel], sqc_handle->osc.oct_offset);
		send_audio_stereo(&sqc_handle->osc);
		vTaskDelay(100 / portTICK_PERIOD_MS);
	}
}

