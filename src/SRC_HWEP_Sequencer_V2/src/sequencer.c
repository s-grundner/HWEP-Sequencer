#include "sequencer.h"

static void sw_cb(void *args)
{

}

esp_err_t sequencer_init(sequencer_config_t **out_sqc_cfg)
{
	esp_err_t err = ESP_OK;
	sequencer_config_t* sqc_cfg = (sequencer_config_t*) malloc(sizeof(sequencer_config_t));
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
		.position = 18,
		.sw_max = 4,
		.sw_state = 0,
	};
	encoder_init(&ec);

	// ------------------------------------------------------------
	// STP LED Driver
	// ------------------------------------------------------------
	stp16cp05_handle_t stp_handle;
	stp16cp05_config_t stp_config = {
		.cs_io = CS_STP16CP05,
		.host = VSPI,
		.miso_io = VSPIQ,
		.mosi_io = VSPID,
	};
	stp16cp05_init(&stp_handle, &stp_config);

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
	adc088s052_init(&adc_handle, &adc_cfg);

	*sqc_cfg = (sequencer_config_t){
		.adc_handle = adc_handle,
		.encoder_handle = &ec,
		.stp_handle = stp_handle,
		.channel = 0,
		.cur_appmode = APP_MODE_BPM,
		.cur_bpm = 120,
		.cur_stp_upper = 0x00,
		.osc = {
			.wavetable = 0,
			.pitch = 1,
			.sample_pos = 0,
			.oct_offset = 0,
		},
		.reset_at_n = 0,
	};
	*out_sqc_cfg = sqc_cfg;

	return ESP_OK;
}