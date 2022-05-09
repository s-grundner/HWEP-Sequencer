/**
 * @file	synth.c
 * @author	@h-ihninger
 * @author	@s-grundner
 * @brief	Library for Synth and Wavetable Processing
 * @version 0.1
 * @date 	2022-05-05
 *
 * @copyright Copyright (c) 2022
 */

#include "synth.h"

// ------------------------------------------------------------
// calculated wavetable pointers
// ------------------------------------------------------------
float *waveFormLookUp[WAVEFORM_TYPE_COUNT];

esp_err_t init_wavetables(void)
{
	float *sine = (float *)malloc(sizeof(float) * WT_SIZE);
	float *saw = (float *)malloc(sizeof(float) * WT_SIZE);
	float *square = (float *)malloc(sizeof(float) * WT_SIZE);
	float *tri = (float *)malloc(sizeof(float) * WT_SIZE);
	float *silence = (float *)malloc(sizeof(float) * WT_SIZE);

	if (!(sine && saw && square && tri && silence))
		return ESP_ERR_NO_MEM;

	for (int i = 0; i < WT_SIZE; i++)
	{
		float val = (float)sin(i * 2.0 * PI / WT_SIZE);
		sine[i] = val;
		saw[i] = (2.0f * ((float)i) / ((float)WT_SIZE)) - 1.0f;
		square[i] = (i > (WT_SIZE / 2)) ? 1 : -1;
		tri[i] = ((i > (WT_SIZE / 2)) ? (((4.0f * (float)i) / ((float)WT_SIZE)) - 1.0f) : (3.0f - ((4.0f * (float)i) / ((float)WT_SIZE)))) - 2.0f;
		silence[i] = 0;
	}

	waveFormLookUp[SINE_WT] = sine;
	waveFormLookUp[SAW_WT] = saw;
	waveFormLookUp[SQUARE_WT] = square;
	waveFormLookUp[TRI_WT] = tri;
	waveFormLookUp[SILENCE] = silence;
	return ESP_OK;
}

void exit_wavetables(void)
{
	free(waveFormLookUp[SINE_WT]);
	free(waveFormLookUp[SAW_WT]);
	free(waveFormLookUp[SQUARE_WT]);
	free(waveFormLookUp[TRI_WT]);
	free(waveFormLookUp[SILENCE]);
}

float *get_wavetable(int index)
{
	return waveFormLookUp[index];
}

// ------------------------------------------------------------
// Audio Processing
// ------------------------------------------------------------

esp_err_t send_audio_stereo(oscillator_t *osc)
{
	size_t i2s_bytes_write = 0;
	uint sample_val = 0;
	double indexIncr = ((WT_SIZE / SAMPLE_RATE) * osc->pitch);
	int *samples_data = malloc(DATA_SIZE);
	if (samples_data == NULL)
		return ESP_ERR_NO_MEM;
	uint16_t mul = ((1 << AUDIO_RESOLUTION_BIT) / 10) - 1;

	for (int n = 0; n < WT_SIZE; n++)
	{
		sample_val = 0;
		sample_val += (uint16_t)(mul * interpol_float(osc->wavetable, osc->sample_pos));
		sample_val = sample_val << 16;
		sample_val += (uint16_t)(mul * interpol_float(osc->wavetable, osc->sample_pos));
		samples_data[n] = sample_val;

		osc->sample_pos += indexIncr;
		if (osc->sample_pos >= (double)WT_SIZE)
			osc->sample_pos -= (double)WT_SIZE;
	}
	ESP_ERROR_CHECK(i2s_write(I2S_NUM, samples_data, DATA_SIZE, &i2s_bytes_write, portMAX_DELAY));
	free(samples_data);
	return ESP_OK;
}

sample_t process_sample(oscillator_t **osc, uint8_t osc_cnt)
{
	sample_t ret = {
		.fl_sample = NULL,
		.fr_sample = NULL,
	};
	// uint16_t mul = ((1 << AUDIO_RESOLUTION_BIT) / 10) - 1;
	for (int i = 0; i < osc_cnt; i++)
	{
		double indexIncr = ((WT_SIZE / SAMPLE_RATE) * osc[i]->pitch);
		uint16_t mul = ((1 << AUDIO_RESOLUTION_BIT) / 12) - 1;

		for (int j = 0; j < WT_SIZE; j++)
		{
			ret.fr_sample[j] += (uint16_t)(mul * interpol_float(osc[i]->wavetable, osc[i]->sample_pos));
			ret.fl_sample[j] += (uint16_t)(mul * interpol_float(osc[i]->wavetable, osc[i]->sample_pos));

			osc[i]->sample_pos += indexIncr;
			if (osc[i]->sample_pos >= (double)WT_SIZE)
				osc[i]->sample_pos -= (double)WT_SIZE;
		}
	}
	return ret;
}

float interpol_float(float *wt, double index)
{
	int indexBase = floor(index);
	double indexFract = index - indexBase;
	float value1 = wt[indexBase];
	float value2 = wt[indexBase + 1];
	return value1 + ((value2 - value1) * indexFract);
}

uint16_t interpol_int(uint16_t *wt, double index)
{
	int indexBase = floor(index);
	double indexFract = index - indexBase;
	uint16_t value1 = wt[indexBase];
	uint16_t value2 = wt[indexBase + 1];
	return value1 + ((value2 - value1) * indexFract);
}

// ------------------------------------------------------------
// I2S functions
// ------------------------------------------------------------

void i2s_init(void)
{
	i2s_config_t i2s_bus_cfg = {
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

	i2s_pin_config_t i2s_pin_cfg = {
		.bck_io_num = 4,
		.ws_io_num = 17,
		.data_out_num = 16,
		.data_in_num = -1,
	};

	ESP_ERROR_CHECK(i2s_driver_install(I2S_NUM, &i2s_bus_cfg, 0, NULL));
	ESP_ERROR_CHECK(i2s_set_pin(I2S_NUM, &i2s_pin_cfg));
	ESP_ERROR_CHECK(i2s_reset());
}

esp_err_t i2s_reset(void)
{
	return (i2s_set_clk(I2S_NUM, SAMPLE_RATE, AUDIO_RESOLUTION_BIT, I2S_CHANNEL_STEREO));
}