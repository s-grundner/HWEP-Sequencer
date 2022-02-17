#include "config.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "esp_intr_alloc.h"

void app_main(void)
{
    esp_err_t err;

    // ------------------------------------------------------------
    // SPI Bus
    // ------------------------------------------------------------

    err = spi_bus_initialize(VSPI_HOST, &vspi_pin_cfg, 0);
    ESP_ERROR_CHECK(err);

    mcp23s08_handle_t mcp_handle = NULL;
    mcp23s08_config_t mcp_cfg = {
        .host = VSPI_HOST,
        .miso_io = VSPIQ,
        .mosi_io = VSPID,
        .cs_io = CS_MCP23S08,
        .intr_io = MCP23S08_INTR,
    };

    err = mcp23s08_init(&mcp_handle, &mcp_cfg);
    ESP_ERROR_CHECK(err);

    err = mcp23s08_write(mcp_handle, HW_ADR_0, IODIR, 0xff);
    ESP_ERROR_CHECK(err);

    // ------------------------------------------------------------
    // ADC
    // ------------------------------------------------------------

    // adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_DB_0);
    // adc1_config_width(ADC_WIDTH_BIT_12);

    // ------------------------------------------------------------
    // Timer
    // ------------------------------------------------------------

    // ------------------------------------------------------------
    // Interrupts
    // ------------------------------------------------------------

    esp_intr_alloc(ETS_GPIO_INTR_SOURCE, ESP_INTR_FLAG_EDGE, NULL, NULL, NULL);

    uint8_t *data = (uint8_t *)malloc(sizeof(uint8_t *));

    while (1)
    {
        err = mcp23s08_read(mcp_handle, HW_ADR_0, GPIO_R, data);
        ESP_ERROR_CHECK(err);
        printf("%d\n", *data);
        vTaskDelay(10);
    }
}