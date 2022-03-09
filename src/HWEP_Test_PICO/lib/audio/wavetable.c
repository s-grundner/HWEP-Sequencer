#include "wavetable.h"

// ------------------------------------------------------------
// calculated wavetable pointers
// ------------------------------------------------------------
float *waveFormLookUp[WAVEFORM_TYPE_COUNT];

void init_wavetables(void)
{
	float *sine = (float *)malloc(sizeof(float) * WT_SIZE);
	float *saw = (float *)malloc(sizeof(float) * WT_SIZE);
	float *square = (float *)malloc(sizeof(float) * WT_SIZE);
	float *tri = (float *)malloc(sizeof(float) * WT_SIZE);
	float *silence = (float *)malloc(sizeof(float) * WT_SIZE);

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