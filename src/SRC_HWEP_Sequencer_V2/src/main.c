#include "config.h"

void app_main(void)
{
    esp_err_t err;
    err = spi_bus_initialize(VSPI_HOST, &v_spi_cfg, 0);
    ESP_ERROR_CHECK(err);

    mcp23s08_handle_t mcp_handle;
    mcp23s08_config_t mcp_cfg = {
        .cs_io = CS_MCP23S08,
        .host = VSPI_HOST,
        .miso_io = VSPIQ,
        .mosi_io = VSPID,
    };

    err = mcp23s08_init(&mcp_handle, &mcp_cfg);
    ESP_ERROR_CHECK(err);

    printf("%d\n",mcp_handle->cfg.cs_io);

    err = mcp23s08_write(mcp_handle, HW_ADR_0, IODIR, 0x00);
    ESP_ERROR_CHECK(err);

    char text[10] = "I LuV yoU ";

    while (1)
    {

        for (int i = 0; i < sizeof(text)/sizeof(text[0]); i++)
        {
            err = mcp23s08_write(mcp_handle, HW_ADR_0, GPIO_R, get_char_segment(text[i]));
            ESP_ERROR_CHECK(err);
            vTaskDelay(30);
            // printf("%d\n",mcp_handle->cfg.cs_io);
        }


        // for (int i = 0; i < 10; i++)
        // {
        //     err = mcp23s08_write(mcp_handle, HW_ADR_0, GPIO_R, cool_anim[i]);
        //     ESP_ERROR_CHECK(err);
        //     vTaskDelay(100);
        // }
    }
}