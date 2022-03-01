#ifndef __ENCODER_H_
#define __ENCODER_H_

#include "esp_system.h"
#include "driver/gpio.h"
#include "esp_intr_alloc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

typedef void (*sw_cb_fn_t)(void *args);

typedef struct {
	uint8_t pin_a;
	uint8_t pin_b;
	uint8_t pin_sw;
	sw_cb_fn_t sw_callback;
	
} encoder_config_t;

typedef struct
{
	uint8_t state;
	uint32_t position;
	uint8_t sw_state;
	encoder_config_t cgf;
} encoder_states_t;

// ------------------------------------------------------------
// Encoder functions
// ------------------------------------------------------------

void encoder_init(encoder_states_t *ec);
int32_t encoder_read(encoder_states_t *arg);
void encoder_write(encoder_states_t *arg, int32_t position);
uint8_t encoder_read_sw(encoder_states_t *arg);

#endif // __ENCODER_H_