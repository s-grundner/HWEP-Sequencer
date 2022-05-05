/*!
* @author	@s-grundner
* @date 	03.05.2022
* @brief	Sequencer Main Header  
*/

#pragma once

#include "esp_system.h"

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

volatile struct sequencer_config_s
{
	// dev handles
	stp16cp05_handle_t stp_handle;
	encoder_context_t* encoder_handle;
	adc088s052_handle_t adc_handle;

	// adc specific data
	uint8_t cur_adc_data[ADC0880S052_CHANNEL_MAX];
	uint8_t channel;

	// stp specific data
	uint8_t cur_stp_upper;

	// Audio data
	oscillator_t osc;
	uint8_t cur_bpm;
	uint8_t oct_offs;

	// general
	app_mode_t cur_appmode;
	int32_t encoder_positions[MAX_APP_MODES];
	uint8_t reset_at_n;
	
};

typedef struct sequencer_config_s sequencer_config_t;
typedef struct sequencer_config_s* sequencer_handle_t;

esp_err_t sequencer_init(sequencer_handle_t *sqc_handle);