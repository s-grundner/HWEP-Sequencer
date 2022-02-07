#pragma once

#ifndef __WAVETABLES_H_
#define __WAVETABLES_H_

#include "config.h"
#include <math.h>

#define WAVEFORM_BIT 10UL
#define WAVEFORM_CNT (1<<WAVEFORM_BIT)
#define WAVEFORM_TYPE_COUNT 5

#define SINE_WT 0x0
#define SAW_WT 0x1
#define SQUARE_WT 0x2
#define TRI_WT 0x3
#define SILENCE 0x4

void init_wavetables(void);
float *get_wavetable(int index);

#endif // #ifndef 