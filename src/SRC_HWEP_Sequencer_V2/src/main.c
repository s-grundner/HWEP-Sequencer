
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

#define CHASE_SPEED_MS (100)

static const char *TAG = "sequencer_main";

static void fsm(void *args)
{
	sequencer_handle_t sqc_handle = (sequencer_handle_t)args;
	int32_t ec_changed[ADC0880S052_CHANNEL_MAX];
	int32_t shift_ec_changed[ADC0880S052_CHANNEL_MAX];

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
		encoder_write(sqc_handle->encoder_handle, 0);

		

		// Main Statemachine
		switch (sqc_handle->cur_appmode)
		{
		case APP_MODE_BPM:
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
				// update wavetable
				sqc_handle->osc.wt_index = abs(sqc_handle->shift_encoder_positions[sqc_handle->cur_appmode] / 2) % WAVEFORM_TYPE_COUNT;
				sqc_handle->osc.wavetable = get_wavetable(sqc_handle->osc.wt_index);

				// reset changed state
				shift_ec_changed[sqc_handle->cur_appmode] = sqc_handle->shift_encoder_positions[sqc_handle->cur_appmode];
			}

			break;
		case APP_MODE_KEY:
			// reset at n index takes effect
			sqc_handle->reset_at_n = (((sqc_handle->encoder_positions[APP_MODE_ENR] >> 1) % 8) + 1);

			// check if encoder data has changed
			if (ec_changed[sqc_handle->cur_appmode] != sqc_handle->encoder_positions[sqc_handle->cur_appmode])
			{
				sqc_handle->cur_key = abs(sqc_handle->encoder_positions[sqc_handle->cur_appmode] >> 1) % 12;
				// reset changed state
				ec_changed[sqc_handle->cur_appmode] = sqc_handle->encoder_positions[sqc_handle->cur_appmode];
			}

			// check if shift-encoder-data has changed
			if (shift_ec_changed[sqc_handle->cur_appmode] != sqc_handle->shift_encoder_positions[sqc_handle->cur_appmode])
			{
				sqc_handle->cur_modal = abs(sqc_handle->shift_encoder_positions[sqc_handle->cur_appmode] >> 1) % 4;
				// reset changed state
				shift_ec_changed[sqc_handle->cur_appmode] = sqc_handle->shift_encoder_positions[sqc_handle->cur_appmode];
			}

			break;
		case APP_MODE_ENR:
			break;
		case APP_MODE_TSP:
			// reset at n index takes effect
			sqc_handle->reset_at_n = (((sqc_handle->encoder_positions[APP_MODE_ENR] >> 1) % 8) + 1);

			// check if encoder data has changed
			if (ec_changed[sqc_handle->cur_appmode] != sqc_handle->encoder_positions[sqc_handle->cur_appmode])
			{
				sqc_handle->osc.transpose = sqc_handle->encoder_positions[sqc_handle->cur_appmode] / 2;
				if (sqc_handle->osc.transpose >= 12)
				{
					sqc_handle->osc.transpose -= 12;
					sqc_handle->osc.oct_offset += 1;
				}
				else if (sqc_handle->osc.transpose <= -12)
				{
					sqc_handle->osc.transpose += 12;
					sqc_handle->osc.oct_offset -= 1;
				}
				// reset changed state
				ec_changed[sqc_handle->cur_appmode] = sqc_handle->encoder_positions[sqc_handle->cur_appmode];
			}

			// check if shift-encoder-data has changed
			if (shift_ec_changed[sqc_handle->cur_appmode] != sqc_handle->shift_encoder_positions[sqc_handle->cur_appmode])
			{
				sqc_handle->osc.oct_offset = abs(sqc_handle->shift_encoder_positions[sqc_handle->cur_appmode] >> 1) % 8; // 8 max octave
				// reset changed state
				shift_ec_changed[sqc_handle->cur_appmode] = sqc_handle->shift_encoder_positions[sqc_handle->cur_appmode];
			}

			break;
		default:
			break;
		}

		// manage audio
		manage_audio(sqc_handle);

		// manage display
		if (sqc_handle->sseg_handle->sseg_refreshable)
		{
			if (sqc_handle->shkey_flag)
			{
				sseg_write(sqc_handle->sseg_handle, get_key_name(get_key_num(sqc_handle->osc.pitch)));
			}
			else
			{
				char str[] = "   ";
				switch (sqc_handle->cur_appmode)
				{
				case APP_MODE_BPM:
					// write display data
					sseg_write(sqc_handle->sseg_handle, sqc_handle->btn_shift ? get_wt_name(sqc_handle->osc.wt_index) : itoa(sqc_handle->cur_bpm, str, 10));
					break;
				case APP_MODE_KEY:
					// write display data
					sseg_write(sqc_handle->sseg_handle, sqc_handle->btn_shift ? get_modal_name(sqc_handle->cur_modal) : get_key_name(sqc_handle->cur_key));
					break;
				case APP_MODE_ENR:
					break;
				case APP_MODE_TSP:
					// write display data
					sseg_write(sqc_handle->sseg_handle, sqc_handle->btn_shift ? itoa(sqc_handle->osc.oct_offset, str, 10) : itoa(sqc_handle->osc.transpose, str, 10));
					break;
				default:
					break;
				}
			}
		}
		// manage_display(sqc_handle);

		// manage_ws2812(sqc_handle);

		vTaskDelay(10 / portTICK_PERIOD_MS);
	}
}

static void led_task(void *args)
{
	uint32_t red = 0;
	uint32_t green = 0;
	uint32_t blue = 0;
	uint16_t hue = 0;
	uint16_t start_rgb = 0;

	rmt_config_t config = RMT_DEFAULT_CONFIG_TX(13, RMT_TX_CHANNEL);
	// set counter clock to 40MHz
	config.clk_div = 2;

	ESP_ERROR_CHECK(rmt_config(&config));
	ESP_ERROR_CHECK(rmt_driver_install(config.channel, 0, 0));

	// install ws2812 driver
	led_strip_config_t strip_config = LED_STRIP_DEFAULT_CONFIG(12, (led_strip_dev_t)config.channel);
	led_strip_t *strip = led_strip_new_rmt_ws2812(&strip_config);
	if (!strip)
	{
		ESP_LOGE(TAG, "install WS2812 driver failed");
	}
	// Clear LED strip (turn off all LEDs)
	ESP_ERROR_CHECK(strip->clear(strip, 100));
	// Show simple rainbow chasing pattern
	ESP_LOGI(TAG, "LED Rainbow Chase Start");

	while (true)
	{
		for (int i = 0; i < 3; i++)
		{
			for (int j = i; j < 12; j += 3)
			{
				// Build RGB values
				hue = j * 360 / 12 + start_rgb;
				led_strip_hsv2rgb(hue, 100, 100, &red, &green, &blue);
				// Write RGB values to strip driver
				ESP_ERROR_CHECK(strip->set_pixel(strip, j, red * 0.1, green * 0.1, blue * 0.1));
			}
			// Flush RGB values to LEDs
			ESP_ERROR_CHECK(strip->refresh(strip, 100));
			vTaskDelay(pdMS_TO_TICKS(CHASE_SPEED_MS));
		}
		start_rgb += 60;
	}
}

void app_main(void)
{
	sequencer_handle_t sqc_handle;
	ESP_ERROR_CHECK(sequencer_init(&sqc_handle));

	xTaskCreate(led_task, "ws_led_task", 2048, NULL, 10, NULL);
	xTaskCreatePinnedToCore(&fsm, "main_fsm", 2048, sqc_handle, 10, NULL, 0);
	while (1)
		vTaskDelay(10 / portTICK_PERIOD_MS);
}