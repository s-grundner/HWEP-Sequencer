#pragma once

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2s.h"
#include "driver/gpio.h"
#include "esp_system.h"
#include "esp_log.h"
#include <math.h>

// ------------------------------------------------------------
// define Constants
// ------------------------------------------------------------

#define PI (3.14159265)

// ------------------------------------------------------------
// Audio Settings
// ------------------------------------------------------------

#define SAMPLE_RATE (44100)
#define SAMPLE_BUFFER_SIZE 48

#define I2S_NUM 0

// #define SAMPLE_SIZE_16BIT
#define SAMPLE_SIZE_24BIT
// #define SAMPLE_SIZE_32BIT


// ------------------------------------------------------------
// Wavetable Settings
// ------------------------------------------------------------

#define SINE_WT 0x0
#define SAW_WT 0x1
#define SQUARE_WT 0x2
#define TRI_WT 0x3
#define SILENCE 0x4

#define WAVEFORM_BIT 10UL
#define WAVEFORM_CNT (1<<WAVEFORM_BIT)
#define WAVEFORM_TYPE_COUNT 5

// ------------------------------------------------------------
// Functions
// ------------------------------------------------------------

void init_wavetables(void);
void i2s_init(i2s_pin_config_t*);

float *get_wavetable(int index);

uint8_t i2s_write_sample_16ch2(float *sample);
uint8_t i2s_write_sample_24ch2(uint8_t *sample);
uint8_t i2s_write_sample_32ch2(uint8_t *sample);
uint8_t i2s_write_stereo(float *fl_sample, float *fr_sample);
uint8_t i2s_write_stereo_buff(float *fl_sample, float *fr_sample, const int buffLen);
