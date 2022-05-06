#include "sequencer.h"

static void sw_cb(void *args)
{
}

esp_err_t sequencer_init(sequencer_config_t **out_sqc_cfg)
{
	sequencer_config_t *sqc_cfg = (sequencer_config_t *)malloc(sizeof(sequencer_config_t));
	if (!sqc_cfg)
		return ESP_ERR_NO_MEM;

	// ------------------------------------------------------------
	// Audio
	// ------------------------------------------------------------
	i2s_init();
	init_wavetables();

	// ------------------------------------------------------------
	// Encoder
	// ------------------------------------------------------------
	encoder_context_t ec = {
		.cgf = {
			.pin_a = A,
			.pin_b = B,
			.pin_sw = SW,
			.sw_callback = sw_cb,
		},
		.state = 0,
		.position = 0,
		.sw_max = MAX_APP_MODES - 1,
		.sw_state = 0,
	};
	encoder_init(&ec);

	// ------------------------------------------------------------
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
		.intr_io = MCP23S08_INTR,
		.miso_io = VSPIQ,
		.mosi_io = VSPID,
	};
	ESP_ERROR_CHECK(mcp23s08_init(&mcp_handle, &mcp_cfg));

	// init 7 segment muxing
	// init interrupts on PS chip

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
		.encoder_handle = &ec,
		.stp_handle = stp_handle,
		.channel = 0,
		.cur_appmode = APP_MODE_BPM,
		.cur_bpm = 120,
		.cur_stp_upper = 0x00,
		.osc = {
			.wavetable = SINE_WT,
			.pitch = get_pitch_hz(40),
			.sample_pos = 0,
			.oct_offset = 0,
		},
		.reset_at_n = 0,
		.active_note_mask = 0xff,
	};
	*out_sqc_cfg = sqc_cfg;

	return ESP_OK;
}

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
		abs(sqc_handle->encoder_positions[sqc_handle->encoder_handle->sw_state]) % ADC0880S052_CHANNEL_MAX);
}

esp_err_t sseg_write(sequencer_handle_t sqc_handle, const char *data)
{
}