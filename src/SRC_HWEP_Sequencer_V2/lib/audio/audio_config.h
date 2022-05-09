#ifndef AUDIO_CONFIG_H_
#define AUDIO_CONFIG_H_

// ------------------------------------------------------------
// Constants
// ------------------------------------------------------------
#define PI (3.14159265)

// ------------------------------------------------------------
// Wavetable Settings
// ------------------------------------------------------------
#define WT_BIT 10UL
#define WT_SIZE (1<<WT_BIT)

// ------------------------------------------------------------
// Waveform Settings
// ------------------------------------------------------------
#define SINE_WT 0
#define SAW_WT 1
#define SQUARE_WT 2
#define TRI_WT 3
#define SILENCE 4
#define WAVEFORM_TYPE_COUNT 5

// ------------------------------------------------------------
// Audio Settings
// ------------------------------------------------------------
#define SAMPLE_RATE (44100.0)
#define AUDIO_RESOLUTION_BIT 16
#define DATA_SIZE (((AUDIO_RESOLUTION_BIT + 8) / 16) * WT_SIZE * 4)

// ------------------------------------------------------------
// I2S Settings
// ------------------------------------------------------------
#define I2S_NUM (I2S_NUM_0)
#define I2S_BCK_IO (GPIO_NUM_19)
#define I2S_WS_IO (GPIO_NUM_21)
#define I2S_DO_IO (GPIO_NUM_22)

#endif //AUDIO_CONFIG_H_