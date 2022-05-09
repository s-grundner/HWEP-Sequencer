#pragma once

#ifndef CONFIG_H_
#define CONFIG_H_

#include "esp_system.h"

// ------------------------------------------------------------
// Peripherals
// ------------------------------------------------------------

// ------------------------------------------------------------
// Pinout
// ------------------------------------------------------------

#define PIN_UNUSED (-1)
#define MCP23S08_INTR (GPIO_NUM_22)
#define WS2812_DATA (GPIO_NUM_13)
#define WS2812_STRIP_LEN (12)
#define RMT_TX_CHANNEL RMT_CHANNEL_0
#define PS_FLAG (GPIO_NUM_32)

#define IN_PS_HW_ADR (0b00)
#define S_SEG_HW_ADR (0b01)

#define SW (GPIO_NUM_27)
#define A (GPIO_NUM_12)
#define B (GPIO_NUM_14)

#define VSPI (VSPI_HOST)
#define VSPIQ (GPIO_NUM_19)		// MISO for SPI3 (VSPI) host
#define VSPID (GPIO_NUM_23)		// MOSI for SPI3 (VSPI) host
#define VSPI_MISO (GPIO_NUM_19) // MISO for SPI3 (VSPI) host
#define VSPI_MOSI (GPIO_NUM_23) // MOSI for SPI3 (VSPI) host
#define VCS0 (GPIO_NUM_5)		// CS0 for SPI3 (VSPI) host
#define VSCK (GPIO_NUM_18)		// SCK for SPI3 (VSPI) hosts

#define CS_MCP23S08 (GPIO_NUM_5)
#define CS_ADC0880S052 (GPIO_NUM_15)
#define CS_STP16CP05 (GPIO_NUM_21)

#define SEG_CNT 3

const gpio_num_t *sseg_channel = {GPIO_NUM_33, GPIO_NUM_25, GPIO_NUM_26};
const uint8_t* appmode_names[SEG_CNT] = {"BPM", "KEY", "ENR", "TSP"}; 

#define S_SEG_CHANNEL_MASK (1ULL << 33) | (1ULL << 25) | (1ULL << 26)
// ------------------------------------------------------------
// define Application Modes
// ------------------------------------------------------------

/* Every Mode Starts with an Indicator on the Hex Display until it is updated by the modes tasks */

#define START_BPM 120
#define MAX_APP_MODES 4

typedef enum
{
	APP_MODE_BPM = 0, // Set Beats per Minute
					  // HEX:		shows BPM
					  // Rotary:	changes BPM on Rotation
					  // Event:		SHIFT Rotary: CHange Waveform
					  //			+SHIFT HEX: Shows Selected Waveform
					  // Index/Cur:	Index (Yellow LEDS)
					  // Blue LEDS: Shows on off State

	APP_MODE_KEY = 1, // Key Settings
					  // HEX:		shows Current Key
					  // Rotary:	Change Key (Shift Key Register)
					  // Event:		+SHIFT Rotary: Change Mode
					  // xndex/Cur:	Index (Yellow LEDS)
					  // xlue LEDS: Shows on off State

	APP_MODE_ENR = 2, // Enable and Reset at Index
					  // HEX:		-
					  // Rotary:	Moves Cursor
					  // Event:		Toggles On/Off State at Cursor -> Lights EN Led
					  // Index/Cur.:Cursor (Yellow LEDS)
					  // Blue LEDS: Shows on off State

	APP_MODE_TSP = 3, // Transpose in Key
					  // HEX:		Transposition amount
					  // Rotary:	Transpose +/- 1 ST
					  // Event:		+SHIFT Rotary: Transpose Octave (+/- 12 ST)
					  // Index/Cur.:Index (Yellow LEDS)
					  // Blue LEDS: Shows on off State
} app_mode_t;

// ------------------------------------------------------------
// Ideas
// ------------------------------------------------------------

// Triplets

#endif // #ifndef CONFIG_H_
