#pragma once

#include "config.h"

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
	int8_t mux_e;
	int8_t mux_z;
	int8_t mux_h;
	mcp23s08_hw_adr hw_adr;
	mcp23s08_context_t *mcp_ctx;
}s_seg_context_t;

void s_seg_init(s_seg_context_t *sg_ctx);
void s_seg_shift(s_seg_context_t *sg_ctx, int8_t dir);
void s_seg_write(char *buf);