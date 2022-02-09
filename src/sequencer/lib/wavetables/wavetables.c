#include "wavetables.h"
#include <stdio.h>

// calculated wavetable pointers
// ------------------------------------------------------------
float *waveFormLookUp[WAVEFORM_TYPE_COUNT];

// add waveforms here
// ------------------------------------------------------------
float *sine = NULL;
float *saw = NULL;
float *square = NULL;
float *tri = NULL;
float *silence = NULL;

void init_wavetables(void)
{
	sine = (float *)malloc(sizeof(float) * WAVEFORM_CNT);
	saw = (float *)malloc(sizeof(float) * WAVEFORM_CNT);
	square = (float *)malloc(sizeof(float) * WAVEFORM_CNT);
	tri = (float *)malloc(sizeof(float) * WAVEFORM_CNT);
	silence = (float *)malloc(sizeof(float) * WAVEFORM_CNT);

	for (int i = 0; i < WAVEFORM_CNT; i++)
	{
		float val = (float)sin(i * 2.0 * PI / WAVEFORM_CNT);
		sine[i] = val;
		saw[i] = (2.0f * ((float)i) / ((float)WAVEFORM_CNT)) - 1.0f;
		square[i] = (i > (WAVEFORM_CNT / 2)) ? 1 : -1;
		tri[i] = ((i > (WAVEFORM_CNT / 2)) ? (((4.0f * (float)i) / ((float)WAVEFORM_CNT)) - 1.0f) : (3.0f - ((4.0f * (float)i) / ((float)WAVEFORM_CNT)))) - 2.0f;
		silence[i] = 0;
	}

	waveFormLookUp[SINE_WT] = sine;
	waveFormLookUp[SAW_WT] = saw;
	waveFormLookUp[SQUARE_WT] = square;
	waveFormLookUp[TRI_WT] = tri;
	waveFormLookUp[SILENCE] = silence;
}

float *get_wavetable(int index)
{
	return waveFormLookUp[index];
}