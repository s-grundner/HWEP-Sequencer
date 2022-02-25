#include "encoder.h"

#include "esp_system.h"
#include "driver/gpio.h"
#include "freertos/task.h"

#define EC_A (GPIO_NUM_33)
#define EC_B (GPIO_NUM_32)
#define EC_SW (GPIO_NUM_26)
#define EC_MASK (1ULL << EC_A) | (1ULL << EC_B)

void cb_sw(void *args)
{
	encoder_write((encoder_states_t*)args, 0);
}

void app_main()
{
	encoder_states_t encoder_cfg = {
		.pin_a = EC_A,
		.pin_b = EC_B,
		.pin_sw = EC_SW,
		.position = 0,
		.state = 0,
		.sw_callback = cb_sw,
	};
	encoder_init(&encoder_cfg);
	while (1)
	{
		printf("%d\n", encoder_read(&encoder_cfg));
		vTaskDelay(10);
	}
}