#include "freertos/FreeRTOS.h"
#include "driver/gpio.h"
#include "esp_system.h"

#include "config.h"
#include "i2s_interface.h"
#include "mcp23s08.h"



void app_main(void)
{   

    // i2s_init(&i2s_pin_cfg);

    esp_err_t err;
    err = spi_bus_initialize(VSPI_HOST, &v_spi_cfg, SPI_DMA_CH_AUTO);
    ESP_ERROR_CHECK(err);

    mcp23s08_config_t mcp_cfg = {
        .cs_io = CS_MCP23S08,
        .miso_io = VSPIQ,
        .mosi_io = VSPID,
        .host = VSPI_HOST,
    };

    mcp23s08_handle_t mcp_handle;
    ESP_ERROR_CHECK(err);

    mcp23s08_init(&mcp_handle, &mcp_cfg);

    mcp23s08_write(mcp_handle, HW_ADR_0, IODIR, 0x00);
    mcp23s08_write(mcp_handle, HW_ADR_0, GPIO_R, 0xff);

    while (1)
    {
        vTaskDelay(1);
    }
}