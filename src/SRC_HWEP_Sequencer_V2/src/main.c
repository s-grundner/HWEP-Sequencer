#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2s.h"
#include "driver/gpio.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/rmt.h"
#include "esp_timer.h"
#include <math.h>

#include "config.h"
// #include "sequencer.h"
#include "mcp23s08.h"
#include "misc.h"
#include "encoder.h"
#include "stp16cp05.h"
#include "adc088s052.h"
#include "synth.h"
#include "scale.h"
#include "led_strip.h"

static const char *TAG = "sequencer";

void sw_cb(void *args)
{
}

void 

typedef struct
{
	stp16cp05_handle_t a;
	unsigned char pos;
	encoder_context_t b;
} context;

static void timer_cb(void *args)
{
	context *ctx = (context *)args;
}

void app_main(void)
{
	/*timer
	Encoder
	STP Treiber
	Audio gage wird
	*/

	// ------------------------------------------------------------
	// Audio
	// ------------------------------------------------------------
	i2s_init();
	init_wavetables();
	oscillator_t osc = {
		.wavetable = 0,
		.pitch = 1,
		.sample_pos = 0,
	};
	// ------------------------------------------------------------
	// Encoder
	// ------------------------------------------------------------
	encoder_context_t ec = {
		.cgf = {
			.pin_a = A,
			.pin_b = B,
			.pin_sw = SW,
			.sw_callback = sw_cb,
		},
		.state = 0,
		.position = 18,
		.sw_max = 4,
		.sw_state = 0,
	};
	encoder_init(&ec);
	int prev_pos = encoder_read(&ec) + 1;

	stp16cp05_handle_t stp_handle;
	stp16cp05_config_t stp_config = {
		.cs_io = CS_STP16CP05,
		.host = VSPI,
		.miso_io = VSPIQ,
		.mosi_io = VSPID,
	};

	stp16cp05_init(&stp_handle, &stp_config);

	// ------------------------------------------------------------
	// Timer
	// ------------------------------------------------------------

	context ctx = {
		.a = stp_handle,
		.b = ec,
		.pos = 0,
	};

	esp_timer_handle_t timer_handle;
	esp_timer_create_args_t timer_cfg = {
		.name = "mux",
		.callback = &timer_cb,
		.arg = &ctx,
	};
	ESP_ERROR_CHECK(esp_timer_create(&timer_cfg, &timer_handle));
	ESP_ERROR_CHECK(esp_timer_start_periodic(timer_handle, 1000));
}