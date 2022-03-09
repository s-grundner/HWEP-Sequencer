#pragma once

#ifndef __CONFIG_H_
#define __CONFIG_H_

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

// ------------------------------------------------------------
// Peripherals
// ------------------------------------------------------------

// #include "i2s_interface.h"
// #include "mcp23s08.h"
// #include "adc088s052.h"
// #include "misc.h"
// #include "sequencer.h"

// ------------------------------------------------------------
// Pinout
// ------------------------------------------------------------

#define PIN_UNUSED (-1)
#define MCP23S08_INTR (GPIO_NUM_22)
#define PL_DIN (GPIO_NUM_13)
#define PS_FLAG (GPIO_NUM_32)

#define IN_PS_HW_ADR 0b00
#define S_SEG_HW_ADR 0b01

#define SW (GPIO_NUM_27)
#define A (GPIO_NUM_12)
#define B (GPIO_NUM_14)

#define VSPIQ (GPIO_NUM_19) // MISO for SPI3 (VSPI) host
#define VSPID (GPIO_NUM_23) // MOSI for SPI3 (VSPI) host
#define VCS0 (GPIO_NUM_5)	// CS0 for SPI3 (VSPI) host
#define VCLK (GPIO_NUM_18)	// SCK for SPI3 (VSPI) hosts

#define	CS_MCP23S08 (GPIO_NUM_5)
#define	CS_ADC0880S052 (GPIO_NUM_15)
#define	CS_STP16CP05 (GPIO_NUM_21)

typedef enum
{
	CH = (GPIO_NUM_33),
	CZ = (GPIO_NUM_25),
	CE = (GPIO_NUM_26),
} s_seg_channel_t;

i2s_pin_config_t i2s_pin_cfg = {
	.bck_io_num = GPIO_NUM_4,
	.ws_io_num = GPIO_NUM_17,
	.data_out_num = GPIO_NUM_16,
	.data_in_num = PIN_UNUSED,
};

spi_bus_config_t vspi_pin_cfg = {
	.miso_io_num = VSPIQ,
	.mosi_io_num = VSPID,
	.sclk_io_num = VCLK,
	.quadhd_io_num = PIN_UNUSED,
	.quadwp_io_num = PIN_UNUSED,
	.max_transfer_sz = 32,
};

// ------------------------------------------------------------
// define Application Modes
// ------------------------------------------------------------

/* Every Mode Starts with an Indicator on the Hex Display until it is updated by the modes tasks */

typedef enum
{
	APP_MODE_BPM = 0, // Set Beats per Minute
					  // HEX:		shows BPM
					  // Rotary:	changes BPM on Rotation
					  // Event:		SHIFT Rotary: CHange Waveform
					  //			+SHIFT HEX: Shows Selected Waveform
					  // Index/Cur:	Index

	APP_MODE_KEY = 1, // Key Settings
					  // HEX:		shows Current Key
					  // Rotary:	Change Key (Shift Key Register)
					  // Event:		+SHIFT Rotary: Change Mode
					  // Index/Cur:	Index

	APP_MODE_ENR = 2, // Enable and Reset at Index
					  // HEX:		-
					  // Rotary:	Moves Cursor
					  // Event:		Toggles On/Off State at Cursor -> Lights EN Led
					  // Index/Cur.:Cursor

	APP_MODE_TSP = 3, // Transpose in Key
					  // HEX:		Transposition amount
					  // Rotary:	Transpose +/- 1 ST
					  // Event:		+SHIFT Rotary: Transpose Octave (+/- 12 ST)
					  // Index/Cur.:Index
} app_mode_t;

// ------------------------------------------------------------
// Ideas
// ------------------------------------------------------------

// Triplets

#endif // #ifndef __CONFIG_H_
