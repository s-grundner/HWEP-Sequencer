#include "sequencer.h"

#include "esp_log.h"
static const char *TAG = "sequencer header";

static void switch_cb(void *args)
{
	sequencer_config_t *ctx = (sequencer_config_t *)args;
	ctx->encoder_positions[ctx->cur_appmode] = encoder_read(ctx->encoder_handle);
	ctx->cur_appmode = encoder_read_sw(ctx->encoder_handle);
	encoder_write(ctx->encoder_handle, ctx->encoder_positions[ctx->cur_appmode]);
	ESP_LOGI(TAG, "%d", ctx->cur_appmode);
}
static void rot_cb(void *args) {}

static void sseg_mux(void *args)
{
	sseg_context_t *ctx = (sseg_context_t *)args;

	gpio_set_level(sseg_channel[ctx->channel], 0);
	mcp23s08_write(ctx->mcp_handle, S_SEG_HW_ADR, GPIO_R, ctx->data_buffer[ctx->channel]);
	ctx->channel = (ctx->channel + 1) % SEG_CNT;
	gpio_set_level(sseg_channel[ctx->channel], 1);
}

static void new_appmode(void* args)
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
		.intr_io = MCP23S08_INTR,
		.miso_io = VSPIQ,
		.mosi_io = VSPID,
	};
	ESP_ERROR_CHECK(mcp23s08_init(&mcp_handle, &mcp_cfg));
	ESP_ERROR_CHECK(mcp23s08_write(mcp_handle, 0, IOCON, (1 << HAEN)));
	ESP_ERROR_CHECK(mcp23s08_write(mcp_handle, S_SEG_HW_ADR, IODIR, 0x00));
	ESP_ERROR_CHECK(mcp23s08_write(mcp_handle, IN_PS_HW_ADR, IODIR, 0xff));
	ESP_ERROR_CHECK(mcp23s08_write(mcp_handle, IN_PS_HW_ADR, GPPU, 0x0f));

	// ------------------------------------------------------------
	// Seven Segment Display and PS input Buttons (MCP23S08)
	// ------------------------------------------------------------

	gpio_config_t cf = {
		.mode = GPIO_MODE_OUTPUT,
		.pin_bit_mask = S_SEG_CHANNEL_MASK,
	};
	gpio_config(&cf);

	esp_timer_handle_t timer_handle;
	esp_timer_create_args_t timer_cfg = {
		.name = "mux",
		.callback = &sseg_mux,
		.arg = sqc_cfg,
	};
	ESP_ERROR_CHECK(esp_timer_create(&timer_cfg, &timer_handle));
	ESP_ERROR_CHECK(esp_timer_start_periodic(timer_handle, 1000));
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
		.encoder_handle = encoder_handle,
		.stp_handle = stp_handle,
		.mcp_handle = mcp_handle,

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

uint8_t get_pos_index(sequencer_handle_t sqc_handle)
{
	return 1 << abs(sqc_handle->encoder_positions[sqc_handle->cur_appmode] >> 1) % ADC0880S052_CHANNEL_MAX;
}

uint32_t bpm_to_us(uint16_t bpm)
{
	return 0x3938700 / bpm;
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
		get_pos_index(sqc_handle));
}

esp_err_t sseg_init(sseg_context_t **out_sseg_ctx, sequencer_handle_t sqc_handle)
{

	sseg_context_t *sseg_ctx = (sseg_context_t *)malloc(sizeof(sseg_context_t));
	if (sseg_ctx == NULL)
		return ESP_ERR_NO_MEM;

	esp_timer_handle_t timer_handle;

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
	esp_timer_create_args_t timer_cfg = {
		.name = "mux",
		.callback = &sseg_mux,
		.arg = sseg_ctx,
	};
	ESP_ERROR_CHECK(esp_timer_create(&timer_cfg, &sseg_ctx->mux_timer));
	ESP_ERROR_CHECK(esp_timer_start_periodic(sseg_ctx->mux_timer, 1000));
}

esp_err_t sseg_write(sseg_handle_t sseg_handle, const uint8_t *data)
{
	ESP_ERROR_CHECK(esp_timer_stop(sseg_handle->mux_timer));
	sseg_handle->data_buffer = data;
	ESP_ERROR_CHECK(esp_timer_start_periodic(sseg_handle->mux_timer, 1000));
	return ESP_OK;
}

esp_err_t sseg_new_appmode(sseg_handle_t sseg_handle)
{
	// esp_timer_handle_t new_appm;
	// esp_timer_create_args_t timer_cfg = {
	// 	.name = "new mode",
	// 	.callback = &new_appmode,
	// 	.arg = sseg_handle,
	// };
	// ESP_ERROR_CHECK(esp_timer_create(&timer_cfg, &sseg_ctx->mux_timer));
	// ESP_ERROR_CHECK(esp_timer_start_periodic(sseg_ctx->mux_timer, 1000));
	return ESP_OK;
}
