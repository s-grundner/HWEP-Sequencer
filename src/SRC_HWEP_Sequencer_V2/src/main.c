/* I2S Example

	This example code will output 100Hz sine wave and triangle wave to 2-channel of I2S driver
	Every 5 seconds, it will change bits_per_sample [16, 24, 32] for i2s data

	This example code is in the Public Domain (or CC0 licensed, at your option.)

	Unless required by applicable law or agreed to in writing, this
	software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
	CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2s.h"
#include "driver/gpio.h"
#include "esp_system.h"
#include "esp_log.h"
#include <math.h>

#include "config.h"
// #include "sequencer.h"
#include "mcp23s08.h"
#include "misc.h"
#include "encoder.h"
#include "stp16cp05.h"
#include "adc088s052.h"

void sw_cb(void *arg){};

void itos(char **s, int i)
{
	for (int i = 0; i < 3; i++)
	{
		*s[i] = i / (pow10(i));
	}
}

void app_main(void)
{
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
		.position = 0,
		.state = 0,
		.sw_max = 5,
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

	while (1)
	{
		int pos = encoder_read(&ec)/2;
		if (pos != prev_pos)
		{
			stp16cp05_write(stp_handle, ~(1 << (pos % 8)), 1 << (pos % 8));
			prev_pos = pos;
		}

		for (int i = 0; i < 3; i++)
		{
			gpio_set_level(seg_channels[i], 1);
			mcp23s08_write(mcp_handle, S_SEG_HW_ADR, GPIO_R, get_char_segment(buf[2 - i]));
			vTaskDelay(10 / (portTICK_RATE_MS));
			gpio_set_level(seg_channels[i], 0);
			mcp23s08_write(mcp_handle, S_SEG_HW_ADR, GPIO_R, 0x00);
		}
		// vTaskDelay(100 / portTICK_RATE_MS);
	}
}