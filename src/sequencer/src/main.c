#include "freertos/FreeRTOS.h"
#include "driver/gpio.h"
#include "esp_system.h"
#include "i2s_interface.h"
#include "wavetables.h"

// #define __DEBUG

void app_main(void)
{
    i2s_init();
    init_wavetables();

#ifdef __DEBUG
    for (int i = 0; i < WAVEFORM_CNT; i++)
    {
        printf("%f\n", get_wavetable(SQUARE_WT)[i]);

        // printf("%c", (i % 8) ? (' ') : ('\n'));
    }
    return;
#endif

    while (1)
    {   
        i2s_write_sample_24ch2(get_wavetable(SINE_WT));
        // vTaskDelay(10 / portTICK_RATE_MS);
    }
}