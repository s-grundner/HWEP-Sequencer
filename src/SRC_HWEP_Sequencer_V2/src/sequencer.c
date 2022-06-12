#include "sequencer.h"
#include "esp_log.h"
static const gpio_num_t sseg_channel[3] = {GPIO_NUM_33, GPIO_NUM_25, GPIO_NUM_26};
static const char *TAG = "sequencer header";

#define MUXTIME_US 3000
#define BTN_MASK_EVENT (1 << 0)
#define BTN_MASK_RESET (1 << 1)
#define BTN_MASK_DEFVAL (1 << 2)
#define BTN_MASK_PAUSE (1 << 3)
// ------------------------------------------------------------
// Static functions and callbacks
// ------------------------------------------------------------

static void switch_cb(void *args)
{
	sequencer_config_t *ctx = (sequencer_config_t *)args;
	ctx->encoder_positions[ctx->cur_appmode] = encoder_read(ctx->encoder_handle);

	ctx->cur_appmode = encoder_read_sw(ctx->encoder_handle);
	// sseg_new_appmode(ctx->sseg_handle);

	encoder_write(ctx->encoder_handle, ctx->encoder_positions[ctx->cur_appmode]);
	ESP_LOGD(TAG, "Current mode%d", ctx->cur_appmode);
}

static void rot_cb(void *args) {}

static void mcp_cb(void *args)
{
	sequencer_handle_t sqc_handle = (sequencer_handle_t)args;
	uint8_t mcp_data;
	mcp23s08_read(sqc_handle->mcp_handle, IN_PS_HW_ADR, INTCAP, &mcp_data);
	uint8_t ps_data = mcp_data >> 4;
	uint8_t btn_data = ~mcp_data & 0x0f;

	// prescaler data
	if (ps_data & 0x08)
	{
		sqc_handle->ps_bpm = (1 << ((ps_data & 0x7) - 3));
	}
	else if (ps_data)
	{
		sqc_handle->ps_gate = (1 << (ps_data - 3));
	}
	sqc_handle->btn_event = btn_data & BTN_MASK_EVENT;
	sqc_handle->btn_reset = btn_data & BTN_MASK_RESET;
	sqc_handle->btn_defval = btn_data & BTN_MASK_DEFVAL;
	sqc_handle->btn_pause = btn_data & BTN_MASK_PAUSE;

	ESP_LOGD(TAG, "Event: %i", sqc_handle->btn_event);
	ESP_LOGD(TAG, "Reset: %i", sqc_handle->btn_reset);
	ESP_LOGD(TAG, "DefVal: %i", sqc_handle->btn_defval);
	ESP_LOGD(TAG, "Pause: %i", sqc_handle->btn_pause);

	ESP_LOGD(TAG, "BPM prescaler: %i", sqc_handle->ps_bpm);
	ESP_LOGD(TAG, "Gate prescaler: %i\n", sqc_handle->ps_gate);
}

static void sseg_mux(void *args)
{
	sseg_context_t *ctx = (sseg_context_t *)args;
	gpio_set_level(sseg_channel[ctx->channel], 0);
	ctx->channel = (ctx->channel + 1) % SEG_CNT;
	mcp23s08_write(ctx->mcp_handle, S_SEG_HW_ADR, GPIO_R, get_char_segment(ctx->data_buffer[ctx->channel]));
	gpio_set_level(sseg_channel[ctx->channel], 1);
}

static void new_appmode(void *args)
{
	// sseg_context_t *ctx = (sseg_context_t*) args;
	// TODO
}

// ------------------------------------------------------------
// Overall sequencer functions
// ------------------------------------------------------------

esp_err_t sequencer_init(sequencer_config_t **out_sqc_cfg)
{
	sequencer_config_t *sqc_cfg = (sequencer_config_t *)malloc(sizeof(sequencer_config_t));
	if (!sqc_cfg)
		return ESP_ERR_NO_MEM;

	// ------------------------------------------------------------
	// Audio
	// ------------------------------------------------------------
	i2s_init();
	ESP_ERROR_CHECK(init_wavetables());

	// ------------------------------------------------------------
	// SPI Bus
	// ------------------------------------------------------------
	spi_bus_config_t buscfg = {
		.max_transfer_sz = 32,
		.miso_io_num = VSPIQ,
		.mosi_io_num = VSPID,
		.sclk_io_num = VSCK,
	};
	ESP_ERROR_CHECK(spi_bus_initialize(VSPI, &buscfg, SPI_DMA_CH_AUTO));

	// ------------------------------------------------------------
	// Encoder
	// ------------------------------------------------------------
	encoder_handle_t encoder_handle;
	encoder_config_t ec_cfg = {
		.pin_a = A,
		.pin_b = B,
		.pin_sw = SW,
		.sw_max = MAX_APP_MODES - 1,
		.sw_callback = switch_cb,
		.sw_args = sqc_cfg,
		.rot_callback = rot_cb,
		.rot_args = sqc_cfg,
	};
	ESP_ERROR_CHECK(encoder_init(&encoder_handle, &ec_cfg));

	// ----------------------------------------------------------
	// STP LED Driver
	// ------------------------------------------------------------
	stp16cp05_handle_t stp_handle;
	stp16cp05_config_t stp_cfg = {
		.cs_io = CS_STP16CP05,
		.host = VSPI,
		.miso_io = VSPIQ,
		.mosi_io = VSPID,
	};
	ESP_ERROR_CHECK(stp16cp05_init(&stp_handle, &stp_cfg));

	// ------------------------------------------------------------
	// Seven Segment Display and PS input Buttons (MCP23S08)
	// ------------------------------------------------------------
	mcp23s08_handle_t mcp_handle;
	mcp23s08_config_t mcp_cfg = {
		.cs_io = CS_MCP23S08,
		.host = VSPI,
		.miso_io = VSPIQ,
		.mosi_io = VSPID,
		.intr_io = MCP23S08_INTR,
		.mcp_callback = mcp_cb,
		.mcp_intr_args = sqc_cfg,
	};
	ESP_ERROR_CHECK(mcp23s08_init(&mcp_handle, &mcp_cfg));
	ESP_ERROR_CHECK(mcp23s08_write(mcp_handle, 0, IOCON, (1 << HAEN) | (1 << INTPOL))); // Enable multiple MCP for same spi transaction
	ESP_ERROR_CHECK(mcp23s08_write(mcp_handle, S_SEG_HW_ADR, IODIR, 0x00));				// Set display MCP-GPIO as Output
	ESP_ERROR_CHECK(mcp23s08_write(mcp_handle, IN_PS_HW_ADR, IODIR, 0xff));				// Set Button MCP-GPIO as Input (Default as Input)
	ESP_ERROR_CHECK(mcp23s08_write(mcp_handle, IN_PS_HW_ADR, GPPU, 0x0f));				// Enable Internal Pullups for Buttons (not prio buttons)

	// Interrupt Settings for Input MCP
	ESP_ERROR_CHECK(mcp23s08_write(mcp_handle, IN_PS_HW_ADR, DEFVAL, 0x0f)); // Set default values
	ESP_ERROR_CHECK(mcp23s08_write(mcp_handle, IN_PS_HW_ADR, INTCON, 0x00)); // Interrupt on change

	ESP_ERROR_CHECK(mcp23s08_write(mcp_handle, IN_PS_HW_ADR, GPINTEN, 0xff)); // Enables Interrupts for all GPI
	ESP_ERROR_CHECK(mcp23s08_dump_intr(mcp_handle, IN_PS_HW_ADR));
	// ------------------------------------------------------------
	// Seven Segment Display and PS input Buttons (MCP23S08)
	// ------------------------------------------------------------

	sseg_handle_t sseg_handle;
	sseg_init(&sseg_handle, sqc_cfg);

	// init interrupts on PS chip
	// on interrupt, read INTCAP register

	// ------------------------------------------------------------
	// ADC
	// ------------------------------------------------------------
	adc088s052_handle_t adc_handle;
	adc088s052_config_t adc_cfg = {
		.cs_io = CS_ADC0880S052,
		.host = VSPI,
		.miso_io = VSPIQ,
		.mosi_io = VSPID,
	};
	ESP_ERROR_CHECK(adc088s052_init(&adc_handle, &adc_cfg));

	*sqc_cfg = (sequencer_config_t){
		.adc_handle = adc_handle,
		.encoder_handle = encoder_handle,
		.stp_handle = stp_handle,
		.mcp_handle = mcp_handle,
		.sseg_handle = sseg_handle,

		.channel = 0,
		.cur_appmode = APP_MODE_BPM,
		.cur_bpm = 120,
		.cur_stp_upper = 0x00,

		.osc = {
			.wavetable = get_wavetable(SINE_WT),
			.pitch = get_pitch_hz(40),
			.sample_pos = 0,
			.oct_offset = 0,
		},

		.reset_at_n = ADC0880S052_CHANNEL_MAX,
		.active_note_mask = 0xff,
	};
	*out_sqc_cfg = sqc_cfg;
	return ESP_OK;
}

// ------------------------------------------------------------
// LED row (upper and lower) specific functions
// ------------------------------------------------------------

esp_err_t stp_index(sequencer_handle_t sqc_handle)
{
	return stp16cp05_write(
		sqc_handle->stp_handle,
		sqc_handle->active_note_mask,
		1 << sqc_handle->channel);
}

esp_err_t stp_cursor(sequencer_handle_t sqc_handle)
{
	return stp16cp05_write(
		sqc_handle->stp_handle,
		sqc_handle->active_note_mask,
		get_pos_index(sqc_handle));
}

// ------------------------------------------------------------
// Seven Segment functions
// ------------------------------------------------------------

esp_err_t sseg_init(sseg_context_t **out_sseg_ctx, sequencer_handle_t sqc_handle)
{
	sseg_context_t *sseg_ctx = (sseg_context_t *)malloc(sizeof(sseg_context_t));
	if (sseg_ctx == NULL)
		return ESP_ERR_NO_MEM;

	gpio_config_t cf = {
		.mode = GPIO_MODE_OUTPUT,
		.pin_bit_mask = S_SEG_CHANNEL_MASK,
	};
	gpio_config(&cf);

	gpio_set_level(SEG_CH_E, 1);
	gpio_set_level(SEG_CH_Z, 1);
	gpio_set_level(SEG_CH_H, 1);

	esp_timer_handle_t timer_handle;
	esp_timer_create_args_t timer_cfg = {
		.name = "mux",
		.callback = &sseg_mux,
		.arg = sseg_ctx,
	};
	ESP_ERROR_CHECK(esp_timer_create(&timer_cfg, &timer_handle));

	*sseg_ctx = (sseg_context_t){
		.channel = 0,
		.data_buffer = malloc(sizeof(uint8_t) * SEG_CNT),
		.mcp_handle = sqc_handle->mcp_handle,
		.mux_timer = timer_handle,
	};
	if (sseg_ctx->data_buffer == NULL)
	{
		free(sseg_ctx);
		return ESP_ERR_NO_MEM;
	}
	sseg_ctx->data_buffer = "SQC";
	ESP_ERROR_CHECK(esp_timer_start_periodic(sseg_ctx->mux_timer, MUXTIME_US));
	*out_sseg_ctx = sseg_ctx;
	return ESP_OK;
}

esp_err_t sseg_write(sseg_context_t *sseg_handle, char *data)
{
	if (strcmp(sseg_handle->data_buffer, data))
	{
		ESP_ERROR_CHECK(esp_timer_stop(sseg_handle->mux_timer));
		sseg_handle->data_buffer = data;
		ESP_ERROR_CHECK(esp_timer_start_periodic(sseg_handle->mux_timer, MUXTIME_US));
	}
	return ESP_OK;
}

esp_err_t sseg_new_appmode(sseg_handle_t sseg_handle)
{
	// start newmode init on segment display
	esp_timer_handle_t new_appm;
	esp_timer_create_args_t timer_cfg = {
		.name = "new mode",
		.callback = &new_appmode,
		.arg = sseg_handle,
	};
	ESP_ERROR_CHECK(esp_timer_create(&timer_cfg, &new_appm));
	ESP_ERROR_CHECK(esp_timer_start_once(new_appm, 2000000));
	return ESP_OK;
}

// ------------------------------------------------------------
// MISC functions
// ------------------------------------------------------------

uint8_t get_pos_index(sequencer_handle_t sqc_handle)
{
	return 1 << abs(sqc_handle->encoder_positions[sqc_handle->cur_appmode] >> 1) % ADC0880S052_CHANNEL_MAX;
}

uint32_t bpm_to_us(uint16_t bpm)
{
	return 0x3938700 / bpm;
}

esp_err_t manage_ws2812(sequencer_handle_t sqc_handle)
{
	// TODO
	switch (sqc_handle->cur_appmode)
	{
	case APP_MODE_BPM:
		break;
	case APP_MODE_KEY:
		break;
	case APP_MODE_ENR:
		break;
	case APP_MODE_TSP:
		break;
	default:
		break;
	}
	return ESP_OK;
}
