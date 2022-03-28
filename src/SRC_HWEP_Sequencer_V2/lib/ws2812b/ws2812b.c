#include "ws2812b.h"

bool rmt_reserved_channels[RMT_CHANNEL_MAX];

void ws2812b_init(ws2812b_handle_t ws_handle, unsigned char new_max_leds)
{
	rmt_channel_t channel = RMT_CHANNEL_MAX;
    for (size_t i = 0; i < RMT_CHANNEL_MAX; i++) {
        if (!rmt_reserved_channels[i]) {
            rmt_reserved_channels[i] = true;
            channel = i;
            break;
        }
    }
    if (channel == RMT_CHANNEL_MAX) {
        // Ran out of channels!
        return;
    }
	rmt_config_t config = RMT_DEFAULT_CONFIG_TX(pin, channel);
    config.clk_div = 2;
	

}
void ws2812b_update_max_leds(ws2812b_handle_t ws_handle, unsigned char new_max_leds)
{

}
void ws2812b_write(ws2812b_handle_t ws_handle, ws2812b_data_t ws_data)
{
	if(ws_handle->max_leds > ws_data.led_num)
	{
		ws2812b_update_max_leds(ws_handle, ws_data.led_num);
	}
	ws_handle->buf[ws_data.led_num] = ws_data;
	for (unsigned char i = 0; i < ws_handle->max_leds; i++)
	{

	}
	
}
void ws2812b_show(ws2812b_handle_t ws_handle)
{

}
void ws2812b_off(ws2812b_handle_t ws_handle)
{

}