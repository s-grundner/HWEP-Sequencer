#ifndef SCALE_H_
#define SCALE_H_

#include "inttypes.h"
#include <math.h>
#include <stdio.h>

#define CONCERT_PITCH 440

double get_pitch_hz(uint8_t key_num);
uint8_t get_key_num(double freq);
void print_key_name(uint8_t key_num);

#endif // SCALE_H_