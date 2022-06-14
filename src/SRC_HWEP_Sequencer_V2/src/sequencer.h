/**
 * @file 	sequencer.h
 * @author	@h-ihninger
 * @author 	@s-grundner
 * @brief	Sequencer Main header
 * @date 	2022-05-05
 * 
 * @copyright Copyright (c) 2022
 */

#pragma once

#include "esp_system.h"
#include "driver/rmt.h"
#include "esp_timer.h"

#include "driver/gpio.h"
#include "driver/i2s.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "driver/adc.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_intr_alloc.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include <config.h>

#include "mcp23s08.h"
#include "misc.h"
#include "encoder.h"
#include "stp16cp05.h"
#include "adc088s052.h"
#include "synth.h"
#include "scale.h"
#include "led_strip.h"

typedef struct {
	char *data_buffer;
	uint8_t channel;
	mcp23s08_handle_t mcp_handle;
	esp_timer_handle_t mux_timer;
} sseg_context_t;

typedef sseg_context_t* sseg_handle_t;

/**
 * @brief	Sequencer Device Configurations and memory for Peripherals
 * @struct 	sequencer_config_s
 */
typedef struct sequencer_config_s
{
	// dev handles
	stp16cp05_handle_t stp_handle; 
	encoder_handle_t encoder_handle;
	adc088s052_handle_t adc_handle;
	mcp23s08_handle_t mcp_handle;
	sseg_handle_t sseg_handle;

	esp_timer_handle_t bpm_timer;

	// adc data
	uint16_t cur_adc_data[ADC0880S052_CHANNEL_MAX];
	uint8_t channel; // 0 to max channels

	// stp data
	uint8_t cur_stp_upper;

	// Audio data
	oscillator_t osc;

	// general
	uint16_t cur_bpm;
	app_mode_t cur_appmode;
	int32_t encoder_positions[MAX_APP_MODES];
	uint8_t reset_at_n;
	uint8_t active_note_mask;

	// buttons
	bool btn_event;
	bool btn_pause;
	bool btn_reset;
	bool btn_defval;

	// prescaler
	uint8_t ps_bpm;
	uint8_t ps_gate;
} sequencer_config_t;
typedef sequencer_config_t* sequencer_handle_t;

/**
 * @brief Initialises all busses and thier Peripherals
 * 
 * @param out_sqc_cfg external Sequencer Configuration to point to
 * @return esp_err_t 
 */
esp_err_t sequencer_init(sequencer_handle_t *sqc_handle);

/**
 * @brief Stops and frees all peripherals
 * 
 * @param sqc_handle External handle
 */
void sequencer_exit(sequencer_handle_t sqc_handle);

/**
 * @brief Shows the Channel Index on the blue LED row
 * 
 * @param sqc_handle External handle
 * @return esp_err_t 
 */
esp_err_t stp_index(sequencer_handle_t sqc_handle);

/**
 * @brief Shows Cursor/Selector on the blue LED row
 * 
 * @param sqc_handle 
 * @return esp_err_t 
 */
esp_err_t stp_cursor(sequencer_handle_t sqc_handle);

/**
 * @brief Initialises 7-Segment display  
 * 
 * @param out_sseg_ctx 
 * @param sqc_handle 
 * @return esp_err_t 
 */
esp_err_t sseg_init(sseg_handle_t *out_sseg_ctx, sequencer_handle_t sqc_handle);

/**
 * @brief writes a data buffer (3 CHARS!) into the 
 * permanent display buffer of the 7-segment multiplexer
 * 
 * @param sseg_handle	7-segment handle struct. Contains databuffer 
 * @param data 			Data to write into the 7-segment handler
 * @return esp_err_t 
 */
esp_err_t sseg_write(sseg_handle_t sseg_handle, char *data);

/**
 * @brief Displays the new selected Mode to the segment display and clears after 2 seconds if the display is currently on standby
 * 
 * @param sqc_handle Extern handle
 * @return esp_err_t 
 */
esp_err_t sseg_new_appmode(sseg_handle_t sseg_handle);

/**
 * @brief Calculates the channel index from the position of the encoder
 * 
 * @param sqc_handle External handle
 * @return uint8_t 
 */
uint8_t get_pos_index(sequencer_handle_t sqc_handle);

/**
 * @brief Converts beat frequency into timeperiod in microseconds
 * 
 * @param bpm Beats Per Minute to convert
 * @return uint32_t 
 */
uint32_t bpm_to_us(uint16_t bpm);

/**
 * @brief takes care of the RGB-LED animations
 * 
 * @param sqc_handle sequencer data
 * @return esp_err_t 
 */
esp_err_t manage_ws2812(sequencer_handle_t sqc_handle);

/**
 * @brief stops current bpm timer and restarts it with the base bpm + the encoder position
 * 
 * @param sqc_handle 
 */
void update_bpm(sequencer_handle_t sqc_handle);