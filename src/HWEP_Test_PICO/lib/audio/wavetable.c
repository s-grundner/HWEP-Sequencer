#include "wavetable.h"

// ------------------------------------------------------------
// calculated wavetable pointers
// ------------------------------------------------------------
float *waveFormLookUp[WAVEFORM_TYPE_COUNT];

// ------------------------------------------------------------
// add waveforms here
// ------------------------------------------------------------
float *sine = NULL;
float *saw = NULL;
float *square = NULL;
float *tri = NULL;
float *silence = NULL;

void init_wavetables(void)
{
	sine = (float *)malloc(sizeof(float) * WT_SIZE);
	saw = (float *)malloc(sizeof(float) * WT_SIZE);
	square = (float *)malloc(sizeof(float) * WT_SIZE);
	tri = (float *)malloc(sizeof(float) * WT_SIZE);
	silence = (float *)malloc(sizeof(float) * WT_SIZE);

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
	free(sine);
	free(saw);
	free(square);
	free(tri);
	free(silence);
}

float *get_wavetable(int index)
{
	return waveFormLookUp[index];
}

float interpol_float(float *wt, double index)
{
	int indexBase = floor(index);
	double indexFract = index - indexBase;
	float value1 = sine[indexBase];
	float value2 = sine[indexBase + 1];
	return value1 + ((value2 - value1) * indexFract);	
}

uint16_t interpol_int(uint16_t *wt, double index)
{
	int indexBase = floor(index);
	double indexFract = index - indexBase;
	uint16_t value1 = sine[indexBase];
	uint16_t value2 = sine[indexBase + 1];
	return value1 + ((value2 - value1) * indexFract);	
}