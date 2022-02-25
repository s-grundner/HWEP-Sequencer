#ifndef __ENCODER_H_
#define __ENCODER_H_

#include "esp_system.h"
#include "driver/gpio.h"
#include "esp_intr_alloc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

typedef void (*sw_cb_fn_t)(void *args);

typedef struct
{
	uint8_t pin_a;
	uint8_t pin_b;
	uint8_t pin_sw;

	uint8_t state;
	uint32_t position;
	uint8_t sw_state;

	sw_cb_fn_t sw_callback;
} encoder_states_t;

// ------------------------------------------------------------
// Encoder functions
// ------------------------------------------------------------

void encoder_init(encoder_states_t *ec);

inline int32_t encoder_read(encoder_states_t *arg)
{
	gpio_intr_disable(arg->pin_a);
	gpio_intr_disable(arg->pin_b);
	int32_t ret = arg->position;
	gpio_intr_enable(arg->pin_a);
	gpio_intr_enable(arg->pin_b);
	return ret;
}
inline void encoder_write(encoder_states_t *arg, int32_t position)
{
	gpio_intr_disable(arg->pin_a);
	gpio_intr_disable(arg->pin_b);
	arg->position = position;
	gpio_intr_enable(arg->pin_a);
	gpio_intr_enable(arg->pin_b);
}
inline uint8_t encoder_read_sw(encoder_states_t *arg)
{
	gpio_intr_disable(arg->pin_sw);
	uint8_t ret = arg->sw_state;
	gpio_intr_enable(arg->pin_sw);
	return ret;
}

#endif // __ENCODER_H_