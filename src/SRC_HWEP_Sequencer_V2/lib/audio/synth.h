/**
 * @file	synth.c 
 * @author	@s-grundner
 * @brief	Library for Synth and Wavetable Processing
 * @version 0.1
 * @date 	2022-05-05
 * 
 * @copyright Copyright (c) 2022
 */

#ifndef SYNTH_H_
#define SYNTH_H_

#include <stdio.h>
#include <math.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2s.h"
#include "driver/gpio.h"
#include "esp_system.h"

#include "audio_config.h"

// ------------------------------------------------------------
// Oscillators
// ------------------------------------------------------------

#define MAX_POLY 1

/**
 * @brief	Oscillator struct with some basic parameters
 * @typedef oscillator_t 
 */
typedef struct
{
	float *wavetable;
	uint8_t wt_index;
	double sample_pos;
	double pitch;
	uint8_t oct_offset;
	uint8_t transpose;
	float detune;
	float pan;
	float level;
} oscillator_t;
// oscillator_t poly_osc[MAX_POLY];

typedef struct
{
	float *fl_sample, *fr_sample;
}sample_t;

// ------------------------------------------------------------
// Wavetable functions
// ------------------------------------------------------------

/**
 * @brief Calculates Basic Wavetables now to save Processing Time
 * 
 * @return esp_err_t
 */
esp_err_t init_wavetables(void);

/**
 * @brief Get the wavetable of a Waveform
 * 
 * @param index		WT index defined by macros
 * @return float* a pointer to the Wavetable
 */
float *get_wavetable(uint8_t index);

char *get_wt_name(uint8_t wt_index);

// ------------------------------------------------------------
// Audio Processing
// ------------------------------------------------------------

/**
 * @brief Interpolates the 2 closest values in a Wavetable
 * 
 * @param wt		Sample as float Pointer to first Samplepoint
 * @param index 	Non-integral intex in the wavetable to interpolate
 * @return float 	Returns (32-bit) float sample point
 */
float interpol_float(float *wt, double index);

/**
 * @brief Interpolates the 2 closest values in a Wavetable
 * 
 * @param wt		Sample as integer Pointer to first Samplepoint
 * @param index 	Non-integral intex in the wavetable to interpolate
 * @return uint16_t Returns interpolated 16 bit sample point
 */
uint16_t interpol_int(uint16_t *wt, double index);

/**
 * @brief Processes Multiple Oscillator-Wavetables into one stereo sample
 * 
 * @param osc		Pointer to first Oscillator
 * @param osc_cnt 	Poly Oscillator Count
 * @return sample_t returns new WT Sample Point
 */
sample_t process_sample(oscillator_t **osc, uint8_t osc_cnt);

// ------------------------------------------------------------
// I2S functions
// ------------------------------------------------------------
void i2s_init(void);
esp_err_t i2s_reset(void);
esp_err_t send_audio_stereo(oscillator_t *osc);
void send_audio_mono(float *f_sample);

#endif  //SYNTH_H_
