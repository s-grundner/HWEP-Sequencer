#ifndef ENCODER_H_
#define ENCODER_H_

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

struct encoder_context
{
	uint8_t state;
	uint32_t position;
	uint32_t sw_state;
	uint32_t sw_max;
	encoder_config_t cgf;
};
typedef struct encoder_context encoder_context_t;
typedef struct encoder_context* encoder_handle_t;
// ------------------------------------------------------------
// Encoder functions
// ------------------------------------------------------------

void encoder_init(encoder_context_t *ec);
int32_t encoder_read(encoder_context_t *arg);
void encoder_write(encoder_context_t *arg, int32_t position);
uint8_t encoder_read_sw(encoder_context_t *arg);

#endif // ENCODER_H_