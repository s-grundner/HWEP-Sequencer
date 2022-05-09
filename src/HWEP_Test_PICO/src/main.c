#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2s.h"
#include "driver/gpio.h"
#include "esp_system.h"
#include "esp_log.h"
#include <math.h>

#include "scale.h"
#include "synth.h"
#include "encoder.h"

void sw_cb(void *args){}

void app_main(void)
{
	encoder_states_t ec = {
		.state = 0,
		.position = 18,
		.sw_max = 4,
		.sw_state = 0,
		.cgf = {
			.pin_a = GPIO_NUM_5,
			.pin_b = GPIO_NUM_10,
			.pin_sw = GPIO_NUM_9,
			.sw_callback = sw_cb,
		},
	};
	encoder_init(&ec);

	i2s_init();
	init_wavetables();

	int prev_pos = encoder_read(&ec) + 1;

	oscillator_t osc = {
		.wavetable = 0,
		.pitch = 1,
		.sample_pos = 0,
	};

	oscillator_t chord[3] = {};

	while (1)
	{
		int button = encoder_read_sw(&ec);
		int pos = abs(encoder_read(&ec) / 2);
		pos = pos ? pos : 1;
		if (prev_pos != pos)
		{
			switch (button)
			{
			case 0:
				printf("%d: SINE\n", button);
				break;
			case 1:
				printf("%d: SAW\n", button);
				break;
			case 2:
				printf("%d: SQUARE\n", button);
				break;
			case 3:
				printf("%d: TRI\n", button);
				break;
			case 4:
				printf("%d: MUTE\n", button);
				break;
			default:
				break;
			}
			printf("%d ----- %lf ----- ", pos, get_pitch_hz(pos));
			print_key_name(pos);
			printf("\n");
			prev_pos = pos;
		}

		osc.wavetable = get_wavetable(button);
		osc.pitch = get_pitch_hz(pos);

		// process_sample(&osc);
		send_audio_stereo(&osc);
	}
}