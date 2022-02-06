#pragma once

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2s.h"
#include "driver/gpio.h"
#include "esp_system.h"
#include "esp_log.h"
#include <math.h>

#include "config.h"

#define I2S_NUM (I2S_NUM_0)
#define I2S_BCK_IO (GPIO_NUM_4)
#define I2S_WS_IO (GPIO_NUM_17)
#define I2S_DO_IO (GPIO_NUM_16)
#define I2S_DI_IO (-1)

void i2s_init(void);
void i2s_exit(void);

uint8_t i2s_write_sample_16ch2(float *sample);

uint8_t i2s_write_sample_24ch2(uint8_t *sample);

uint8_t i2s_write_sample_32ch2(uint8_t *sample);

uint8_t i2s_write_stereo(float *fl_sample, float *fr_sample);

uint8_t i2s_write_stereo_buff(float *fl_sample, float *fr_sample, const int buffLen);
