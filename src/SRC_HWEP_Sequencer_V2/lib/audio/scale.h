/**
 * @file 	scale.h
 * @author 	@h-ihninger
 * @author 	@s-grundner
 * @brief 	Basic Functions for Frequency calculation
 * @version 0.1
 * @date 	2022-05-05
 *
 * @copyright Copyright (c) 2022
 */

#ifndef SCALE_H_
#define SCALE_H_

#include "inttypes.h"
#include <math.h>
#include <stdio.h>

#define CONCERT_PITCH 440.0
#define ADC_MAX_VAL 255.0
#define A0_NUM 49

typedef enum
{
	KEY_A = 0,
	KEY_AS = 1,
	KEY_B = 2,
	KEY_C = 3,
	KEY_CS = 4,
	KEY_D = 5,
	KEY_DS = 6,
	KEY_E = 7,
	KEY_F = 8,
	KEY_FS = 9,
	KEY_G = 10,
	KEY_GS  = 11,
} key_names_t;

typedef enum
{
	MAJ = 0, // Major
	MIN = 1, // Minor
	HMJ = 2, // Harmonic Major
	HMN = 3, // Harmonic Minor
} modal_names_t;

double get_pitch_hz(uint8_t key_num);
uint8_t get_key_num(double freq);
void print_key_name(uint8_t key_num);
double adc_to_pitch(uint8_t adc_val, uint8_t oct_offset);
uint8_t adc_to_num(uint8_t adc_val, uint8_t oct_offset);

#endif // SCALE_H_