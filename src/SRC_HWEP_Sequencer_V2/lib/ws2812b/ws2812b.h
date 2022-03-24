#ifndef WS2812B_H_
#define WS2812B_H_

#include "esp_system.h"
#include "driver/rmt.h"

typedef struct {
	unsigned char red;
	unsigned char green;
	unsigned char blue;
	
	unsigned char led_num;
}ws2812b_data_t;

struct ws2812b_context
{
	ws2812b_data_t *buf;
	unsigned char max_leds;

	rmt_config_t *rmt_cfg;
};
typedef struct ws2812b_context ws2812b_context_t;
typedef struct ws2812b_context *ws2812b_handle_t;

void ws2812b_init(ws2812b_handle_t ws_handle, unsigned char new_max_leds);
void ws2812b_update_max_leds(ws2812b_handle_t ws_handle, unsigned char new_max_leds);
void ws2812b_write(ws2812b_handle_t ws_handle, ws2812b_data_t ws_data);
void ws2812b_show(ws2812b_handle_t ws_handle);
void ws2812b_off(ws2812b_handle_t ws_handle);

#endif // WS2812B_H_