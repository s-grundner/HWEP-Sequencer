
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

static void fsm(void *args)
{
	sequencer_handle_t sqc_handle = (sequencer_handle_t)args;
	int32_t ec_changed[ADC0880S052_CHANNEL_MAX];
	int32_t shift_ec_changed[ADC0880S052_CHANNEL_MAX];

	char str[3];
	for (int i = 0; i < ADC0880S052_CHANNEL_MAX; i++)
	{
		ec_changed[i] = encoder_read(sqc_handle->encoder_handle) + 1;
		shift_ec_changed[i] = encoder_read(sqc_handle->encoder_handle) + 1;
	}

	for (;;)
	{
		// store encoder value in sqc struct to save time
		if (sqc_handle->btn_shift)
		{
			sqc_handle->shift_encoder_positions[sqc_handle->cur_appmode] += encoder_read(sqc_handle->encoder_handle);
		}
		else
		{
			sqc_handle->encoder_positions[sqc_handle->cur_appmode] += encoder_read(sqc_handle->encoder_handle);
		}

		// Main Statemachine
		switch (sqc_handle->cur_appmode)
		{
		case APP_MODE_BPM:
			// ESP_ERROR_CHECK(stp_index(sqc_handle));

			// reset at n index takes effect
			sqc_handle->reset_at_n = (((sqc_handle->encoder_positions[APP_MODE_ENR] >> 1) % 8) + 1);

			// check if encoder data has changed
			if (ec_changed[sqc_handle->cur_appmode] != sqc_handle->encoder_positions[sqc_handle->cur_appmode])
			{
				update_bpm(sqc_handle);
				ec_changed[sqc_handle->cur_appmode] = sqc_handle->encoder_positions[sqc_handle->cur_appmode];
			}

			// check if shift-encoder-data has changed
			if (shift_ec_changed[sqc_handle->cur_appmode] != sqc_handle->shift_encoder_positions[sqc_handle->cur_appmode])
			{
				sqc_handle->osc.wt_index = abs(sqc_handle->shift_encoder_positions[sqc_handle->cur_appmode] / 2) % WAVEFORM_TYPE_COUNT;
				sqc_handle->osc.wavetable = get_wavetable(sqc_handle->osc.wt_index);
				shift_ec_changed[sqc_handle->cur_appmode] = sqc_handle->shift_encoder_positions[sqc_handle->cur_appmode];
			}

			// check if segment display is writable
			if (sqc_handle->sseg_handle->sseg_refreshable)
			{
				// write display data
				sseg_write(sqc_handle->sseg_handle, sqc_handle->btn_shift ? get_wt_name(sqc_handle->osc.wt_index) : itoa(sqc_handle->cur_bpm, str, 10));
			}

			break;
		case APP_MODE_KEY:
			// ESP_ERROR_CHECK(stp_index(sqc_handle));

			// reset at n index takes effect
			sqc_handle->reset_at_n = (((sqc_handle->encoder_positions[APP_MODE_ENR] >> 1) % 8) + 1);

			break;
		case APP_MODE_ENR:
			// ESP_ERROR_CHECK(stp_cursor(sqc_handle));
			break;
		case APP_MODE_TSP:
			// ESP_ERROR_CHECK(stp_index(sqc_handle));
			// reset at n index takes effect
			sqc_handle->reset_at_n = (((sqc_handle->encoder_positions[APP_MODE_ENR] >> 1) % 8) + 1);
			break;
		default:
			break;
		}

		encoder_write(sqc_handle->encoder_handle, 0);


		// manage_ws2812(sqc_handle);
		// manage_audio_data(sqc_handle);

		if (sqc_handle->active_note_mask & (1 << sqc_handle->channel))
		{
			send_audio_stereo(&sqc_handle->osc);
		}
	}
}

void app_main(void)
{
	sequencer_handle_t sqc_handle;
	ESP_ERROR_CHECK(sequencer_init(&sqc_handle));

	if (xTaskCreatePinnedToCore(&fsm, "main_fsm", 2048, sqc_handle, 10, NULL, 1) == pdTRUE)
	{
		while (1)
			vTaskDelay(10 / portTICK_PERIOD_MS);
	}
	else
	{
		ESP_LOGE(TAG, "Failed to create Main FSM.\n Abort.");
	}
}