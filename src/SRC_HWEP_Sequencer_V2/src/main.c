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

static const char *TAG = "sequencer_main";

void app_main(void)
{
	sequencer_handle_t sqc_handle;
	ESP_ERROR_CHECK(sequencer_init(&sqc_handle));

	uint32_t ec_changed[ADC0880S052_CHANNEL_MAX];

	for (int i = 0; i < ADC0880S052_CHANNEL_MAX; i++)
		ec_changed[i] = encoder_read(sqc_handle->encoder_handle) + 1;

	while (1)
	{
		// store encoder value in sqc struct to save time
		sqc_handle->encoder_positions[sqc_handle->cur_appmode] = encoder_read(sqc_handle->encoder_handle);

		ESP_LOGD(TAG, "ec_pos = %d", sqc_handle->encoder_positions[sqc_handle->cur_appmode]);
		ESP_LOGD(TAG, "bpm = %d", 0x3938700 / sqc_handle->cur_bpm);

		// Main Statemachine
		switch (sqc_handle->cur_appmode)
		{
		case APP_MODE_BPM:
			ESP_ERROR_CHECK(stp_index(sqc_handle));
			if (ec_changed[sqc_handle->cur_appmode] != sqc_handle->encoder_positions[sqc_handle->cur_appmode])
			{
				ESP_LOGD(TAG, "EC data changed: %i -> %i", ec_changed[sqc_handle->cur_appmode], sqc_handle->encoder_positions[sqc_handle->cur_appmode]);
				update_bpm(sqc_handle);
				ec_changed[sqc_handle->cur_appmode] = sqc_handle->encoder_positions[sqc_handle->cur_appmode];
			}
			// sseg_write(sqc_handle->sseg_handle, "BPM");
			break;
		case APP_MODE_KEY:
			ESP_ERROR_CHECK(stp_index(sqc_handle));
			// sseg_write(sqc_handle->sseg_handle, "KEY");
			break;
		case APP_MODE_ENR:
			ESP_ERROR_CHECK(stp_cursor(sqc_handle));
			// sseg_write(sqc_handle->sseg_handle, "ENR");
			break;
		case APP_MODE_TSP:
			ESP_ERROR_CHECK(stp_index(sqc_handle));
			// sseg_write(sqc_handle->sseg_handle, "TSP");
			break;
		default:
			break;
		}
		manage_ws2812(sqc_handle);
		sqc_handle->osc.pitch = adc_to_pitch(sqc_handle->cur_adc_data[sqc_handle->channel], sqc_handle->osc.oct_offset);
		send_audio_stereo(&sqc_handle->osc);
		vTaskDelay(10 / portTICK_PERIOD_MS);
	}
}
