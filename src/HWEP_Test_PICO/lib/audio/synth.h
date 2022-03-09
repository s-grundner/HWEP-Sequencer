#ifndef SYNTH_H_
#define SYNTH_H_

#include <stdio.h>
#include <math.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2s.h"
#include "driver/gpio.h"
#include "esp_system.h"
#include "esp_log.h"

// ------------------------------------------------------------
// define Constants
// ------------------------------------------------------------
#define PI (3.14159265)

// ------------------------------------------------------------
// Audio Settings
// ------------------------------------------------------------
#define SAMPLE_RATE (44100.0)
#define AUDIO_RESOLUTION_BIT 16

// ------------------------------------------------------------
// Wavetable Settings
// ------------------------------------------------------------
#define SINE_WT 0
#define SAW_WT 1
#define SQUARE_WT 2
#define TRI_WT 3
#define SILENCE 4

#define WT_BIT 10UL
#define WT_SIZE (1<<WT_BIT)
#define WAVEFORM_TYPE_COUNT 5

// ------------------------------------------------------------
// Functions
// ------------------------------------------------------------
void init_wavetables(void);
float *get_wavetable(int index);
float interpol_float(float *wt, double index);
uint16_t interpol_int(uint16_t *wt, double index);

#endif  //SYNTH_H_
