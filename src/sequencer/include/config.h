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
	// Event:	+SHIFT Rotary: 
	//		+SHIFT HEX: Shows Selected Waveform
	// Index/Cur:	Index
#define APP_MODE_KEY 0x01	// Key Settings
	// HEX:		shows Current Key
	// Rotary:	Change Key (Shift Key Register)
	// Event:	+SHIFT Rotary: Change Mode
	// Index/Cur:	Index
#define APP_MODE_ENR 0x02	// Enable at Index and Resets
	// HEX:		-
	// Rotary:	Moves Cursor
	// Event:	Toggles On/Off State at Cursor -> Lights EN Led
	// Index/Cur.:	Cursor
#define APP_MODE_TSP 0x03	// Transpose in Key
	// HEX:		Transposition amount
	// Rotary:	Transpose +/- 1 ST
	// Event:	+SHIFT Rotary: Transpose Octave (+/- 12 ST)
	// Index/Cur.:	Index
// ------------------------------------------------------------
// 
// ------------------------------------------------------------

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
