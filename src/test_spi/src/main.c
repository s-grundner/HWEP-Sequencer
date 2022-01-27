
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "hal/gpio_types.h"
#include "driver/gpio.h"

#define GPIO_OUTPUT_IO_0 CONFIG_GPIO_OUTPUT_0
#define GPIO_OUTPUT_IO_1 CONFIG_GPIO_OUTPUT_1
#define GPIO_OUTPUT_PIN_SEL ((1ULL<<GPIO_OUTPUT_IO_0)|(1ULL<<GPIO_OUTPUT_IO_1))

void app_main()
{
	gpio_config_t io_config = {
		.intr_type = GPIO_INTR_DISABLE,
		.mode = GPIO_MODE_OUTPUT,
		.pin_bit_mask = 0xffffff,
		.pull_down_en = GPIO_PULLDOWN_DISABLE,
		.pull_up_en = GPIO_PULLUP_DISABLE,
	};

	gpio_config(&io_config);
	
}