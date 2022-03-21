#pragma once

#include "esp_system.h"

#include "driver/gpio.h"
#include "driver/i2s.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "driver/adc.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_intr_alloc.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "mcp23s08.h"

#define MUX_E 0
#define MUX_Z 1
#define MUX_H 2
#define MUX_MASK (1ULL<<33)|(1ULL<<25)|(1ULL<<26)

typedef enum {
	START,
	__LAST_STATE
} fsm_states_t;

typedef struct {
	fsm_states_t state;
	fsm_states_t next_state;
} fsm_state_info_t;

void run(void);

// ------------------------------------------------------------
// Seven Segment 
// ------------------------------------------------------------

typedef struct {
	mcp23s08_hw_adr hw_adr;
	mcp23s08_handle_t mcp_handle;
	mcp23s08_config_t mcp_cfg;
	uint32_t *buffer;
}s_seg_context_t;

void s_seg_init(s_seg_context_t *sg_ctx);
void s_seg_shift(s_seg_context_t *sg_ctx, int8_t dir);
void s_seg_write(s_seg_context_t *sg_ctx, char *buf);
void s_seg_write_single(s_seg_context_t *sg_ctx, uint8_t s_seg_channel, char data);   