#pragma once

#ifndef __CONFIG_H_
#define __CONFIG_H_

// ------------------------------------------------------------
// define Application Modes
// ------------------------------------------------------------

/* Every Mode Starts with an Indicator on the Hex Display until it is updated by the modes tasks */

#define APP_MODE_BPM 0x00	// Set Beats per Minute
	// HEX:		shows BPM
	// Rotary:	changes BPM on Rotation
	// KeyRegister:	Default Key Register
	// Event:	-
	// Index/Cur:	Index
#define APP_MODE_KEY 0x01	// Key Settings
	// HEX:		shows Current Key
	// Rotary:	Change Key (Shift Key Register)
	// KeyRegister:	Default Key Register
	// Event:	+SHIFT Rotary: Change Mode
	// Index/Cur:	Index
#define APP_MODE_ENR 0x02	// Enable at Index and Resets
	// HEX:		-
	// Rotary:	Moves Cursor
	// KeyRegister:	Default Key Register
	// Event:	Toggles On/Off State at Cursor -> Lights EN Led
	// Index/Cur.:	Cursor
#define APP_MODE_TSP 0x04	// Transpose in Key
	// HEX:		Transposition amount
	// Rotary:	Transpose +/- 1 ST
	// KeyRegister:	Shows Current Key in Register
	// Event:	+SHIFT Rotary: Transpose Octave (+/- 12 ST)
	// Index/Cur.:	Index
// ------------------------------------------------------------
// 
// ------------------------------------------------------------

// struct key_registerT
// {
// 	uint8_t keyregister;
// 	uint8_t octave;
// 	uint8_t key
// };


// ------------------------------------------------------------
// Audio Settings
// ------------------------------------------------------------

#define SAMPLE_RATE (44100)
#define SAMPLE_BUFFER_SIZE 48

// #define SAMPLE_SIZE_16BIT
#define SAMPLE_SIZE_24BIT
// #define SAMPLE_SIZE_32BIT

// ------------------------------------------------------------
// define Constants
// ------------------------------------------------------------

#define PI (3.14159265)

#endif // #ifndef __CONFIG_H_
