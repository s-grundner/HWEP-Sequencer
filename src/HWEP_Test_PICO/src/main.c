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

#define I2S_NUM (0)
#define I2S_BCK_IO (GPIO_NUM_19)
#define I2S_WS_IO (GPIO_NUM_21)
#define I2S_DO_IO (GPIO_NUM_22)
#define I2S_DI_IO (-1)

void i2s_init(void)
{
	i2s_config_t i2s_config = {
		.mode = I2S_MODE_MASTER | I2S_MODE_TX,
		.sample_rate = SAMPLE_RATE,
		.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
		.channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
		.communication_format = I2S_COMM_FORMAT_STAND_MSB,
		.dma_buf_count = 2,
		.dma_buf_len = 512,
		.tx_desc_auto_clear = true,
		.use_apll = true,
		.intr_alloc_flags = ESP_INTR_FLAG_LEVEL1 // Interrupt level 1
	};
	i2s_pin_config_t pin_config = {
		.bck_io_num = I2S_BCK_IO,
		.ws_io_num = I2S_WS_IO,
		.data_out_num = I2S_DO_IO,
		.data_in_num = I2S_DI_IO // Not used
	};
	i2s_driver_install(I2S_NUM, &i2s_config, 0, NULL);
	i2s_set_pin(I2S_NUM, &pin_config);
}

void i2s_write_clr_desc()
{
	size_t i2s_bytes_write = 0;
	double frequency = 4000;

	uint32_t data_size = ((AUDIO_RESOLUTION_BIT + 8) / 16) * WT_SIZE * 4;

	uint sample_val = 0;

	double frqTL = WT_SIZE / SAMPLE_RATE;
	double indexIncr = (frqTL * frequency);

	int *samples_data = malloc(data_size);

	int cnt = 0;
	double index = 0;
	i2s_set_clk(I2S_NUM, SAMPLE_RATE, AUDIO_RESOLUTION_BIT, I2S_CHANNEL_STEREO);
	while (cnt < 1000)
	{
		
		for (int n = 0; n < WT_SIZE; n++)
		{
			uint16_t mul = ((1 << 16) / 4) - 1;

			sample_val = 0;
			sample_val += (uint16_t)(mul * interpol_float(get_wavetable(0), index));
			sample_val = sample_val << 16;
			sample_val += (uint16_t)(mul * interpol_float(get_wavetable(0), index));
			samples_data[n] = sample_val;

			index += indexIncr;
			if (index >= WT_SIZE)
				index -= (double)WT_SIZE;
		}
		i2s_write(I2S_NUM, samples_data, data_size, &i2s_bytes_write, portMAX_DELAY);
		// vTaskDelay(5 / portTICK_PERIOD_MS);
		// cnt ++;
	}
	free(samples_data);
}

void i2s_write_freq(double frequency, uint8_t wf)
{
	size_t i2s_bytes_write = 0;

	uint16_t samples_per_cycle = SAMPLE_RATE / frequency;
	int total_samples = floor(WT_SIZE/samples_per_cycle)*WT_SIZE;

	uint32_t data_size = ((AUDIO_RESOLUTION_BIT + 8) / 16) * WT_SIZE * 4;
	int *samples_data = malloc(data_size);
	uint sample_val = 0;

	double frqTL = WT_SIZE / SAMPLE_RATE;
	double indexIncr = (frqTL * frequency);
	double index = 0;

	printf("%lf\n%lf\n", frqTL, frqTL * frequency);

	for (int i = 0; i < WT_SIZE; i++)
	{
		samples_data[i] = 0;
	}

	i2s_set_clk(I2S_NUM, SAMPLE_RATE, AUDIO_RESOLUTION_BIT, I2S_CHANNEL_STEREO);
	i2s_write(I2S_NUM, samples_data, data_size, &i2s_bytes_write, portMAX_DELAY);

	for (int n = 0; n < WT_SIZE; n++)
	{
		uint16_t mul = ((1 << 16) / 8) - 1;

		sample_val = 0;
		if(n<total_samples)
		{
			// sample_val += (uint16_t)(mul * (interpol_float(get_wavetable(SQUARE_WT), index)));
			// sample_val = sample_val << 16;
			sample_val += (uint16_t)(mul * get_wavetable(wf)[(int)index]);
		}
		
		samples_data[n] = sample_val;

		// printf("%f ", get_wavetable(SQUARE_WT)[(int)index]);

		index += indexIncr;
		if (index >= WT_SIZE)
			index -= (double)WT_SIZE;

		// printf("%u ", sample_val);
	}
	i2s_set_clk(I2S_NUM, SAMPLE_RATE, AUDIO_RESOLUTION_BIT, I2S_CHANNEL_STEREO);
	i2s_write(I2S_NUM, samples_data, data_size, &i2s_bytes_write, portMAX_DELAY);
	free(samples_data);
}

void sw_cb(void *args)
{
	// (encoder_states_t*)args.sw;
}

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

	i2s_init();

	encoder_init(&ec);
	init_wavetables();

	i2s_write_clr_desc();

	uint16_t prev_pos = encoder_read(&ec) + 1;
	while (1)
	{
		uint32_t button = encoder_read_sw(&ec);
		uint16_t pos = abs(encoder_read(&ec) / 2);
		pos = pos ? pos : 1;
		if (prev_pos != pos)
		{
			switch (encoder_read_sw(&ec))
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
			get_key_name(pos);
			printf("\n");
			i2s_write_freq(get_pitch_hz(pos), button);
			prev_pos = pos;
		}
		vTaskDelay(50 / portTICK_PERIOD_MS);
	}
}