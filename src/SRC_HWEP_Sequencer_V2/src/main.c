#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2s.h"
#include "driver/gpio.h"
#include "esp_system.h"
#include "esp_log.h"
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

typedef struct {
	s_seg_channel_t ch;
	uint8_t data;
	mcp23s08_handle_t mcp_h;

}s_seg_context_t;

void sw_cb(void *arg){


};

void itos(char **s, int i)
{
	for (int i = 0; i < 3; i++)
	{
		*s[i] = i / (pow10(i));
	}
}

static void timer_cb (void* args) {
	gpio_set_level(((s_seg_context_t*)args)->ch, 1);
	mcp23s08_write(((s_seg_context_t*)args)->mcp_h, S_SEG_HW_ADR, GPIO_R, ((s_seg_context_t*)args)->data);
}

void app_main(void)
{
	
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

	s_seg_channel_t seg_channels[3] = {CE, CZ, CH};
	char buf[3] = "SQC";
	gpio_set_level(CE, 1);
	gpio_set_level(CZ, 1);
	gpio_set_level(CH, 1);

	// ------------------------------------------------------------
	// SPI-Bus
	// ------------------------------------------------------------

	spi_bus_config_t buscfg = {
		.max_transfer_sz = 32,
		.miso_io_num = VSPIQ,
		.mosi_io_num = VSPID,
		.sclk_io_num = VSCK,
	};
	spi_bus_initialize(VSPI, &buscfg, SPI_DMA_CH_AUTO);

	// ------------------------------------------------------------
	// ADC
	// ------------------------------------------------------------

	// adc088s052_handle_t adc_handle;
	// adc088s052_config_t adc_cfg = {
	// 	.cs_io = CS_ADC0880S052,
	// 	.host = VSPI,
	// 	.miso_io = VSPIQ,
	// 	.mosi_io = VSPID,
	// };
	// adc088s052_init(&adc_handle, &adc_cfg);
	//ich hasse alles
	// ------------------------------------------------------------
	// STP16CP05
	// ------------------------------------------------------------

	stp16cp05_handle_t stp_handle;
	stp16cp05_config_t stp_cfg = {
		.cs_io = CS_STP16CP05,
		.host = VSPI,
		.mosi_io = VSPID,
		.miso_io = VSPIQ,
	};
	stp16cp05_init(&stp_handle, &stp_cfg);

	stp16cp05_write(stp_handle, 0x00, 0x00);

	// ------------------------------------------------------------
	// MCP23S08
	// ------------------------------------------------------------
	mcp23s08_handle_t mcp_handle;
	mcp23s08_config_t mcp_cfg = {
		.cs_io = CS_MCP23S08,
		.host = VSPI,
		.miso_io = VSPIQ,
		.mosi_io = VSPID,
		.intr_io = -1,
	};
	mcp23s08_init(&mcp_handle, &mcp_cfg);

	mcp23s08_write(mcp_handle, 0, IOCON, (1 << HAEN));
	mcp23s08_write(mcp_handle, S_SEG_HW_ADR, IODIR, 0x00);
	mcp23s08_write(mcp_handle, IN_PS_HW_ADR, IODIR, 0xff);
	mcp23s08_write(mcp_handle, IN_PS_HW_ADR, GPPU, 0x0f);

	mcp23s08_write(mcp_handle, S_SEG_HW_ADR, GPIO_R, get_char_segment('A'));

	// ------------------------------------------------------------
	//	Timer
	// ------------------------------------------------------------
	esp_timer_init();
	s_seg_context_t s_seg_ctx = {
		.ch = CE,
		.data = 0x00,
		.mcp_h = mcp_handle,
	};

	esp_timer_handle_t timer_handle;
	esp_timer_create_args_t timer_cfg = {
		.name = "mux",
		.callback = &timer_cb,
		.arg = &s_seg_ctx,
	};
	esp_timer_create(&timer_cfg, &timer_handle);
	esp_timer_start_periodic(timer_handle,10);

	while (1)
	{
		for (int i = 0; i < 3; i++)
		{
			gpio_set_level(seg_channels[i], 1);
			mcp23s08_write(mcp_handle, S_SEG_HW_ADR, GPIO_R, get_char_segment(buf[2 - i]));
			
			int button = encoder_read_sw(&ec);
			int pos = abs(encoder_read(&ec) / 2);
			pos = pos ? pos : 1;
			if (prev_pos != pos)
			{
				stp16cp05_write(stp_handle, ~(1 << (pos % 8)), 1 << (pos % 8));
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

			osc.wavetable = get_wavetable(button);
			osc.pitch = get_pitch_hz(pos);

			send_audio_stereo(&osc);

			gpio_set_level(seg_channels[i], 0);
			mcp23s08_write(mcp_handle, S_SEG_HW_ADR, GPIO_R, 0x00);
		}
	}
}