/**
 * @file 	sequencer.h
 * @author	@h-ihninger
 * @author 	@s-grundner
 * @brief	Sequencer Main header
 * @version 0.1
 * @date 	2022-05-05
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once


#include "esp_system.h"
#include "esp_log.h"
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

#define bpmtous(a) (0x3938700 / (a))


/**
 * @brief	Sequencer Device Configurations and memory for Peripherals
 * @struct 	sequencer_config_s
 * @param	stp_handle		
 * @param	encoder_handle	
 * @param	adc_handle		
 * 
 * @param	cur_adc_data	
 * @param	channel			
 * 
 * @param 	cur_stp_upper	
 * @param	osc				
 * @param 	cur_bpm			
 * @param	cur_appmode
 * @param	encoder_positions
 * @param	reset_at_n
 */
struct sequencer_config_s
{
	// dev handles
	stp16cp05_handle_t stp_handle;
	encoder_context_t* encoder_handle;
	adc088s052_handle_t adc_handle;

	// adc specific data
	uint16_t cur_adc_data[ADC0880S052_CHANNEL_MAX];
	uint8_t channel; // 0 to max channels

	// stp specific data
	uint8_t cur_stp_upper;

	// Audio data
	oscillator_t osc;
	uint8_t cur_bpm;

	// general
	app_mode_t cur_appmode;
	int32_t encoder_positions[MAX_APP_MODES];
	uint8_t reset_at_n;
	uint8_t active_note_mask;
	
};
typedef struct sequencer_config_s sequencer_config_t;
typedef struct sequencer_config_s* sequencer_handle_t;

/**
 * @brief Initialises all busses and thier Peripherals
 * 
 * @param out_sqc_cfg external Sequencer Configuration to point to
 * @return esp_err_t 
 */
esp_err_t sequencer_init(sequencer_handle_t *sqc_handle);

esp_err_t stp_index(sequencer_handle_t sqc_handle);
esp_err_t stp_cursor(sequencer_handle_t sqc_handle);

esp_err_t sseg_write(sequencer_handle_t sqc_handle, const char* data);