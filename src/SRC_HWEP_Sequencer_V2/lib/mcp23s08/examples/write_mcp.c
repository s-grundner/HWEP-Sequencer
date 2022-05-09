#include "mcp23s08.h"

void app_main(void)
{
	
	spi_bus_config_t buscfg = {
		.max_transfer_sz = 32,
		.miso_io_num = VSPIQ,
		.mosi_io_num = VSPID,
		.sclk_io_num = VSCK,
	};
	mcp23s08_config_t mcpcfg = {
		.cs_io = CS_MCP23S08,
		.host = VSPI,
		.miso_io = VSPIQ,
		.mosi_io = VSPID,
		.intr_io = -1,
	};
	spi_bus_initialize(VSPI, &buscfg, SPI_DMA_CH_AUTO);
	mcp23s08_handle_t mcp_handle;
	mcp23s08_init(&mcp_handle, &mcpcfg);
	mcp23s08_write(mcp_handle, S_SEG_HW_ADR, IODIR, 0x00);
	mcp23s08_write(mcp_handle, S_SEG_HW_ADR, GPIO_R, 0xAA);
	
	while (1)
	{
        vTaskDelay(50/portTICK_RATE_MS);
	}