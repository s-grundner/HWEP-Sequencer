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

typedef struct
{
	uint8_t ch;
	uint8_t buf[3];
	mcp23s08_handle_t mcp_h;
	bool is_on;
	s_seg_channel_t ch_gpio[3];
} s_seg_context_t;

static void sw_cb(void *arg)
{
	ESP_LOGI(TAG, "EC_SWITCH_CB");
};

static void timer_cb(void *args)
{
	s_seg_context_t *ctx = (s_seg_context_t *)args;

	if (ctx->is_on)
	{
		gpio_set_level(ctx->ch_gpio[ctx->ch], 1);
		mcp23s08_write(ctx->mcp_h, S_SEG_HW_ADR, GPIO_R, get_char_segment(ctx->buf[ctx->ch]));
		ctx->is_on = false;
	}
	else
	{
		gpio_set_level(ctx->ch_gpio[ctx->ch], 0);
		mcp23s08_write(ctx->mcp_h, S_SEG_HW_ADR, GPIO_R, 0x00);
		if (ctx->ch >= 2)
		{
			ctx->ch = 0;
		}
		else
		{
			ctx->ch++;
		}
		ctx->is_on = true;
	}
}

void app_main(void)
{
	// ------------------------------------------------------------
	// ws2812 ring
	// ------------------------------------------------------------
#define rgb_ring
#ifdef rgb_ring
	rmt_config_t config = RMT_DEFAULT_CONFIG_TX(WS2812_DATA, RMT_TX_CHANNEL);
	// set counter clock to 40MHz
	config.clk_div = 2;

	ESP_ERROR_CHECK(rmt_config(&config));
	ESP_ERROR_CHECK(rmt_driver_install(config.channel, 0, 0));

	// install ws2812 driver
	led_strip_config_t strip_config = LED_STRIP_DEFAULT_CONFIG(WS2812_STRIP_LEN, (led_strip_dev_t)config.channel);
	led_strip_t *strip = led_strip_new_rmt_ws2812(&strip_config);
	if (!strip)
	{
		ESP_LOGE(TAG, "install WS2812 driver failed");
	}
	ESP_ERROR_CHECK(strip->clear(strip, 100));

	uint32_t r, g, b, hue, start_rgb=0;
#endif
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
	// ------------------------------------------------------------
	// Segment Display
	// ------------------------------------------------------------
	gpio_config_t cf = {
		.mode = GPIO_MODE_OUTPUT,
		.pin_bit_mask = S_SEG_CHANNEL_MASK,
	};
	gpio_config(&cf);
	// ------------------------------------------------------------
	// SPI-Bus
	// ------------------------------------------------------------
	spi_bus_config_t buscfg = {
		.max_transfer_sz = 32,
		.miso_io_num = VSPIQ,
		.mosi_io_num = VSPID,
		.sclk_io_num = VSCK,
	};
	ESP_ERROR_CHECK(spi_bus_initialize(VSPI, &buscfg, SPI_DMA_CH_AUTO));
	// ------------------------------------------------------------
	// STP16CP05
	// ------------------------------------------------------------
#define stp
#ifdef stp
	stp16cp05_handle_t stp_handle;
	stp16cp05_config_t stp_cfg = {
		.cs_io = CS_STP16CP05,
		.host = VSPI,
		.mosi_io = VSPID,
		.miso_io = VSPIQ,
	};
	ESP_ERROR_CHECK(stp16cp05_init(&stp_handle, &stp_cfg));

	stp16cp05_write(stp_handle, 0x00, 0x00);
#endif
	// ------------------------------------------------------------
	// MCP23S08
	// ------------------------------------------------------------
#define mcp
#ifdef mcp
	mcp23s08_handle_t mcp_handle;
	mcp23s08_config_t mcp_cfg = {
		.cs_io = CS_MCP23S08,
		.host = VSPI,
		.miso_io = VSPIQ,
		.mosi_io = VSPID,
		.intr_io = -1,
	};

	ESP_ERROR_CHECK(mcp23s08_init(&mcp_handle, &mcp_cfg));

	ESP_ERROR_CHECK(mcp23s08_write(mcp_handle, 0, IOCON, (1 << HAEN)));
	ESP_ERROR_CHECK(mcp23s08_write(mcp_handle, S_SEG_HW_ADR, IODIR, 0x00));
	ESP_ERROR_CHECK(mcp23s08_write(mcp_handle, IN_PS_HW_ADR, IODIR, 0xff));
	ESP_ERROR_CHECK(mcp23s08_write(mcp_handle, IN_PS_HW_ADR, GPPU, 0x0f));
#endif
	// ------------------------------------------------------------
	// ADC
	// ------------------------------------------------------------
#define adc
#ifdef adc
	adc088s052_handle_t adc_handle;
	adc088s052_config_t adc_cfg = {
		.cs_io = CS_ADC0880S052,
		.host = VSPI,
		.miso_io = VSPIQ,
		.mosi_io = VSPID,
	};
	ESP_ERROR_CHECK(adc088s052_init(&adc_handle, &adc_cfg));
	uint16_t adc_data[8];
	uint16_t adc_data_prev[8];
	bool data_changed;
#endif
	// ------------------------------------------------------------
	//	Timer
	// ------------------------------------------------------------
#define timer
#ifdef timer
	s_seg_context_t seg_ctx = {
		.buf = "SQC",
		.ch = 0,
		.mcp_h = mcp_handle,
		.is_on = true,
		.ch_gpio = {CH, CZ, CE},
	};
	esp_timer_handle_t timer_handle;
	esp_timer_create_args_t timer_cfg = {
		.name = "mux",
		.callback = &timer_cb,
		.arg = &seg_ctx,
	};
	ESP_ERROR_CHECK(esp_timer_create(&timer_cfg, &timer_handle));
	ESP_ERROR_CHECK(esp_timer_start_periodic(timer_handle, 1000));
#endif

	while (1)
	{
#ifdef adc
		for (int i = 0; i < ADC0880S052_CHANNEL_MAX; i++)
		{
			ESP_ERROR_CHECK(adc088s052_get_raw(adc_handle, i, &adc_data[i]));
			if (adc_data_prev[i] != adc_data[i])
			{
				data_changed = true;
				adc_data_prev[i] = adc_data[i];
			}
		}
		if (data_changed)
		{
			data_changed = false;
			for (int i = 0; i < ADC0880S052_CHANNEL_MAX; i++)
			{
				printf("| %03x ", adc_data[i]);
			}
			printf("|\n");
		}
#endif
		int button = encoder_read_sw(&ec);
		int pos = abs(encoder_read(&ec) / 2);
		pos = pos ? pos : 1;
		if (prev_pos != pos)
		{
#ifdef stp
			ESP_ERROR_CHECK(stp16cp05_write(stp_handle, ~(1 << (pos % 8)), 1 << (pos % 8)));
#endif
			switch (button)
			{
			case 0:
				printf("%d: SINE\n", button);
				break;
			case 1:
				printf("%d: SAW\n", button);
				break;
			case 2:
				printf("%d: SQUARE\n", button);
				break;
			case 3:
				printf("%d: TRI\n", button);
				break;
			case 4:
				printf("%d: MUTE\n", button);
				break;
			default:
				break;
			}
			printf("%d ----- %lf ----- ", pos, get_pitch_hz(pos));
			print_key_name(pos);
			printf("\n");
			prev_pos = pos;
		}

#ifdef rgb_ring
		for (int i = 0; i < WS2812_STRIP_LEN; i++)
		{
				hue = pos%12 * 360 / 12 + start_rgb;
                led_strip_hsv2rgb(hue, 100, 100, &r, &g, &b);
                // Write RGB values to strip driver
                ESP_ERROR_CHECK(strip->set_pixel(strip, i, r*0.5, g*0.5, b*0.5));
				ESP_ERROR_CHECK(strip->set_pixel(strip, pos%12, ~(uint32_t)(r*0.5), ~(uint32_t)(g*0.5), ~(uint32_t)(b*0.5)));
		}
		ESP_ERROR_CHECK(strip->refresh(strip, 100));
		// start_rgb += 60;
#endif

		osc.wavetable = get_wavetable(button);
		osc.pitch = get_pitch_hz(pos);

		send_audio_stereo(&osc);
	}
}