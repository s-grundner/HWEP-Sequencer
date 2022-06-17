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

#define CONCERT_PITCH	440.0
#define ADC_MAX_VAL 	255.0
#define A0_NUM			49

double get_pitch_hz(uint8_t key_num);
uint8_t get_key_num(double freq);
void print_key_name(uint8_t key_num);
double adc_to_pitch(uint8_t adc_val, uint8_t oct_offset);
uint8_t adc_to_num(uint8_t adc_val, uint8_t oct_offset);

#endif // SCALE_H_