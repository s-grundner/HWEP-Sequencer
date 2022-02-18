#include "config.h"

#define INTR_MASK (1ULL << GPIO_NUM_32)

volatile mcp23s08_handle_t mcp_handle = NULL;

static QueueHandle_t gpio_evt_queue = NULL;

static void IRAM_ATTR gpio_isr_handler(void *arg)
{
    uint32_t gpio_num = (uint32_t)arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

static void gpio_task_example(void *arg)
{
    uint32_t io_num;
    for (;;)
    {
        if (xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY))
        {
            uint8_t data = 0;
            esp_err_t err = mcp23s08_read(mcp_handle, HW_ADR_0, GPIO_R, &data);
            ESP_ERROR_CHECK(err);
            printf("%x\n", data);
            printf("GPIO[%d] intr, val: %d\n", io_num, gpio_get_level(io_num));
        }
    }
}

void app_main(void)
{
    esp_err_t err;

    // ------------------------------------------------------------
    // Interrupts
    // ------------------------------------------------------------

    gpio_config_t intr_cfg = {
        .intr_type = GPIO_INTR_POSEDGE,
        .pull_down_en = 1,
        .pull_up_en = 0,
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = 1ULL << GPIO_NUM_32,
    };
    gpio_config(&intr_cfg);
    gpio_set_intr_type(GPIO_NUM_32, GPIO_INTR_POSEDGE);
    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    xTaskCreate(gpio_task_example, "gpio_task_example", 2048, NULL, 10, NULL);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(GPIO_NUM_32, gpio_isr_handler, (void *)GPIO_NUM_32);

    // ------------------------------------------------------------
    // SPI Bus
    // ------------------------------------------------------------

    err = spi_bus_initialize(VSPI_HOST, &vspi_pin_cfg, 0);
    ESP_ERROR_CHECK(err);

    mcp23s08_config_t mcp_cfg = {
        .host = VSPI_HOST,
        .miso_io = VSPIQ,
        .mosi_io = VSPID,
        .cs_io = CS_MCP23S08,
        .intr_io = -1,
    };

    err = mcp23s08_init(&mcp_handle, &mcp_cfg);
    ESP_ERROR_CHECK(err);

    err = mcp23s08_write(mcp_handle, HW_ADR_0, IODIR, 0xff);
    ESP_ERROR_CHECK(err);

    // err = mcp23s08_write(mcp_handle, HW_ADR_0, , 0xff);
    // ESP_ERROR_CHECK(err);

    // uint8_t data = 0;

    // ------------------------------------------------------------
    // ADC
    // ------------------------------------------------------------

    // adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_DB_0);
    // adc1_config_width(ADC_WIDTH_BIT_12);

    // ------------------------------------------------------------
    // Timer
    // ------------------------------------------------------------

    // gpio_isr_handler_remove(GPIO_NUM_32);
    uint8_t data = 0;

    while (1)
    {
        vTaskDelay(10);
    }
}