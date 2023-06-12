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

typedef void (*sw_cb_fn_t)(void *args);	 // switch callback function
typedef void (*rot_cb_fn_t)(void *args); // rotary callback function

/**
 * @brief Encoder configuration
 * @typedef encoder_config_t
 * @param pin_a encoder GPI A
 * @param pin_b encoder GPI B
 * @param pin_sw encoder switch GPI
 * @param sw_max max. value for integrated switch-pressed counter
 * @param init_position initial relative position
 * @param sw_callback user switch callback function
 * @param rot_callback user rotary callback function
 * @param sw_args user switch callback function arguments
 * @param rot_args user rotary callback function arguments
 */
typedef struct encoder_config
{
	uint8_t pin_a;
	uint8_t pin_b;
	uint8_t pin_sw;

	uint32_t sw_max;
	int32_t init_position;

	sw_cb_fn_t sw_callback;
	rot_cb_fn_t rot_callback;
	void *sw_args;
	void *rot_args;
} encoder_config_t;

/**
 * @brief Encoder context
 * @typedef encoder_context_t
 * @param state encoder state (internal)
 * @param position encoder position (relative) read with function encoder_read(...)
 * @param sw_state encoder switch state (internal) read with function encoder_read_sw(...)
 * @param cfg encoder configuration
 */
struct encoder_context
{
	uint8_t state;
	uint32_t position;
	uint32_t sw_state;
	encoder_config_t cfg;
};
typedef struct encoder_context encoder_context_t;
typedef struct encoder_context *encoder_handle_t;
// ------------------------------------------------------------
// Encoder functions
// ------------------------------------------------------------

/**
 * @brief Initialize the encoder
 *
 * @param ec encoder handle
 * @param ec_cfg encoder configuration
 * @return esp_err_t ESP_OK on success
 */
esp_err_t encoder_init(encoder_handle_t *ec, encoder_config_t *ec_cfg);

/**
 * @brief Read the encoder position
 *
 * @param arg encoder handle
 * @return int32_t encoder position
 */
int32_t encoder_read(encoder_context_t *arg);

/**
 * @brief Set the encoder position to a relative value
 *
 * @param arg encoder handle
 * @param position relative position
 */
void encoder_write(encoder_context_t *arg, int32_t position);

/**
 * @brief Read the encoder switch state
 *
 * @param arg encoder handle
 * @return uint8_t encoder switch state
 */
uint8_t encoder_read_sw(encoder_context_t *arg);

#endif // ENCODER_H_