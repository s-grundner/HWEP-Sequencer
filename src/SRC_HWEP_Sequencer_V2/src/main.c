#include "config.h"

void app_main(void)
{
    // esp_err_t err;

    // // ------------------------------------------------------------
    // // SPI Bus
    // // ------------------------------------------------------------

    // err = spi_bus_initialize(VSPI_HOST, &vspi_pin_cfg, 0);
    // ESP_ERROR_CHECK(err);

    // mcp23s08_handle_t mcp_handle = NULL;
    // mcp23s08_config_t mcp_cfg = {
    //     .host = VSPI_HOST,
    //     .miso_io = VSPIQ,
    //     .mosi_io = VSPID,
    //     .cs_io = CS_MCP23S08,
    //     .intr_io = -1,
    // };

    // err = mcp23s08_init(&mcp_handle, &mcp_cfg);
    // ESP_ERROR_CHECK(err);

    // s_seg_context_t s_seg_ctx = {
    //     .hw_adr = HW_ADR_0,
    //     .mcp_ctx = mcp_handle,
    // };
    // s_seg_init(&s_seg_ctx);

    // // err = mcp23s08_write(mcp_handle, HW_ADR_0, , 0xff);
    // // ESP_ERROR_CHECK(err);

    // // uint8_t data = 0;

    // // ------------------------------------------------------------
    // // ADC
    // // ------------------------------------------------------------

    // // adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_DB_0);
    // // adc1_config_width(ADC_WIDTH_BIT_12);

    // // ------------------------------------------------------------
    // // Timer
    // // ------------------------------------------------------------

    // // gpio_isr_handler_remove(GPIO_NUM_32);

    while (1)
    {
        vTaskDelay(10);
    }
}