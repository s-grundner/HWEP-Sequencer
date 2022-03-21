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

#include "audio_config.h"

// ------------------------------------------------------------
// Oscillators
// ------------------------------------------------------------

#define MAX_POLY 1

typedef struct
{
	float *wavetable;
	double sample_pos;
	double pitch;
	int8_t oct_offset;
	float detune;
	float pan;
	float level;
}oscillator_t;
// oscillator_t poly_osc[MAX_POLY];

typedef struct
{
	float *fl_sample, *fr_sample;
}sample_t;

// ------------------------------------------------------------
// Wavetable functions
// ------------------------------------------------------------
void init_wavetables(void);
float *get_wavetable(int index);

// ------------------------------------------------------------
// Audio Processing
// ------------------------------------------------------------
float interpol_float(float *wt, double index);
uint16_t interpol_int(uint16_t *wt, double index);

sample_t process_sample(oscillator_t **osc, uint8_t osc_cnt);

// ------------------------------------------------------------
// I2S functions
// ------------------------------------------------------------
void i2s_init(void);
void i2s_reset(void);
void send_audio_stereo(oscillator_t *osc);
void send_audio_mono(float *f_sample);

#endif  //SYNTH_H_
