#pragma once

#ifndef __CONFIG_H_
#define __CONFIG_H_

#include "driver/gpio.h"
#include "driver/i2s.h"
#include "driver/spi_master.h"
#include "freertos/FreeRTOS.h"
#include "driver/gpio.h"
#include "esp_system.h"

#include "i2s_interface.h"
#include "mcp23s08.h"
#include "misc.h"

// ------------------------------------------------------------
// Pinout
// ------------------------------------------------------------

#define PIN_UNUSED (-1)
#define MCP23S08_INTR (GPIO_NUM_22)
#define PL_DIN (GPIO_NUM_13)

typedef enum
{
	CH = GPIO_NUM_33,
	CZ = GPIO_NUM_25,
	CE = GPIO_NUM_26,
} seg_t;

typedef enum
{
	SW = GPIO_NUM_27,
	A = GPIO_NUM_12,
	B = GPIO_NUM_14,
} ec11_t;

typedef enum
{
	VSPIQ = GPIO_NUM_19, // MISO for SPI3 (VSPI) host
	VSPID = GPIO_NUM_23, // MOSI for SPI3 (VSPI) host
	VCS0 = GPIO_NUM_5,	 // CS0 for SPI3 (VSPI) host
	VCLK = GPIO_NUM_18,	 // SCK for SPI3 (VSPI) hosts
} spi_pins_t;

typedef enum
{
	CS_MCP23S08 = GPIO_NUM_5,
	CS_ADC0880S052 = GPIO_NUM_15,
	CS_STP16CP05 = GPIO_NUM_21,
} cs_t;

i2s_pin_config_t i2s_pin_cfg = {
	.bck_io_num = GPIO_NUM_4,
	.ws_io_num = GPIO_NUM_17,
	.data_out_num = GPIO_NUM_16,
	.data_in_num = PIN_UNUSED,
};

spi_bus_config_t v_spi_cfg = {
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

#define APP_MODE_BPM 0x00 // Set Beats per Minute
						  // HEX:		shows BPM
						  // Rotary:	changes BPM on Rotation
						  // Event:		SHIFT Rotary: CHange Waveform
						  //			+SHIFT HEX: Shows Selected Waveform
						  // Index/Cur:	Index

#define APP_MODE_KEY 0x01 // Key Settings
						  // HEX:		shows Current Key
						  // Rotary:	Change Key (Shift Key Register)
						  // Event:		+SHIFT Rotary: Change Mode
						  // Index/Cur:	Index

#define APP_MODE_ENR 0x02 // Enable and Reset at Index
						  // HEX:		-
						  // Rotary:	Moves Cursor
						  // Event:		Toggles On/Off State at Cursor -> Lights EN Led
						  // Index/Cur.:Cursor

#define APP_MODE_TSP 0x03 // Transpose in Key
						  // HEX:		Transposition amount
						  // Rotary:	Transpose +/- 1 ST
						  // Event:		+SHIFT Rotary: Transpose Octave (+/- 12 ST)
						  // Index/Cur.:Index

// ------------------------------------------------------------
// Ideas
// ------------------------------------------------------------

// Triplets

#endif // #ifndef __CONFIG_H_
