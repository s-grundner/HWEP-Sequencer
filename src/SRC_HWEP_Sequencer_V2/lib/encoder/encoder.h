/**
 * @file 	encoder.h
 * @author 	@s-grundner
 * @brief 	Library for Rotary Encoders (EC11)
 * @version 0.1
 * @date 	2022-05-06
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef ENCODER_H_
#define ENCODER_H_

#include "esp_system.h"
#include "driver/gpio.h"
#include "esp_intr_alloc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

typedef void (*sw_cb_fn_t)(void *args);
typedef void (*rot_cb_fn_t)(void *args);

typedef struct encoder_config{
	uint8_t pin_a;
	uint8_t pin_b;
	uint8_t pin_sw;

	uint32_t sw_max;
	int32_t init_position;

	sw_cb_fn_t sw_callback;
	rot_cb_fn_t rot_callback;
	void* sw_args; 
	void* rot_args;
} encoder_config_t;

struct encoder_context
{
	uint8_t state;
	uint32_t position;
	uint32_t sw_state;
	encoder_config_t cfg;
};
typedef struct encoder_context encoder_context_t;
typedef struct encoder_context* encoder_handle_t;
// ------------------------------------------------------------
// Encoder functions
// ------------------------------------------------------------

esp_err_t encoder_init(encoder_handle_t *ec, encoder_config_t *ec_cfg);
int32_t encoder_read(encoder_context_t *arg);
void encoder_write(encoder_context_t *arg, int32_t position);
uint8_t encoder_read_sw(encoder_context_t *arg);

#endif // ENCODER_H_