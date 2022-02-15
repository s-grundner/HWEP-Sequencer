#include "i2s_interface.h"

// ------------------------------------------------------------
// calculated wavetable pointers
// ------------------------------------------------------------
float *waveFormLookUp[WAVEFORM_TYPE_COUNT];

// ------------------------------------------------------------
// add waveforms here
// ------------------------------------------------------------
float *sine = NULL;
float *saw = NULL;
float *square = NULL;
float *tri = NULL;
float *silence = NULL;

// ------------------------------------------------------------
// Functions
// ------------------------------------------------------------

esp_err_t ret;

void i2s_init(i2s_pin_config_t *pin_config)
{
    i2s_config_t i2s_config = {
        .mode = I2S_MODE_MASTER | I2S_MODE_TX,
        .sample_rate = SAMPLE_RATE,
#ifdef SAMPLE_SIZE_16BIT
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
#endif
#ifdef SAMPLE_SIZE_24BIT
        .bits_per_sample = I2S_BITS_PER_SAMPLE_24BIT,
#endif
#ifdef SAMPLE_SIZE_32BIT
        .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
#endif
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_MSB,
        .dma_buf_count = 8,
        .dma_buf_len = 64,
        .use_apll = false,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1 // Interrupt level 1
    };

    ret=i2s_driver_install(I2S_NUM, &i2s_config, 0, NULL);
    ESP_ERROR_CHECK(ret);
    ret=i2s_set_pin(I2S_NUM, pin_config);
    ESP_ERROR_CHECK(ret);
    ret=i2s_set_sample_rates(I2S_NUM, SAMPLE_RATE);
    ESP_ERROR_CHECK(ret);
    ret=i2s_set_clk(I2S_NUM, SAMPLE_RATE, 32, 2);
    ESP_ERROR_CHECK(ret);
    ret=i2s_start(I2S_NUM);
    ESP_ERROR_CHECK(ret);
}


void init_wavetables(void)
{
	sine = (float *)malloc(sizeof(float) * WAVEFORM_CNT);
	saw = (float *)malloc(sizeof(float) * WAVEFORM_CNT);
	square = (float *)malloc(sizeof(float) * WAVEFORM_CNT);
	tri = (float *)malloc(sizeof(float) * WAVEFORM_CNT);
	silence = (float *)malloc(sizeof(float) * WAVEFORM_CNT);

	for (int i = 0; i < WAVEFORM_CNT; i++)
	{
		float val = (float)sin(i * 2.0 * PI / WAVEFORM_CNT);
		sine[i] = val;
		saw[i] = (2.0f * ((float)i) / ((float)WAVEFORM_CNT)) - 1.0f;
		square[i] = (i > (WAVEFORM_CNT / 2)) ? 1 : -1;
		tri[i] = ((i > (WAVEFORM_CNT / 2)) ? (((4.0f * (float)i) / ((float)WAVEFORM_CNT)) - 1.0f) : (3.0f - ((4.0f * (float)i) / ((float)WAVEFORM_CNT)))) - 2.0f;
		silence[i] = 0;
	}

	waveFormLookUp[SINE_WT] = sine;
	waveFormLookUp[SAW_WT] = saw;
	waveFormLookUp[SQUARE_WT] = square;
	waveFormLookUp[TRI_WT] = tri;
	waveFormLookUp[SILENCE] = silence;
}

float *get_wavetable(int index)
{
	return waveFormLookUp[index];
}

uint8_t i2s_write_sample_16ch2(float *sample)
{   
    static size_t bytes_written1 = 0;
    i2s_write(I2S_NUM, (&sample[0]), 8, &bytes_written1, portMAX_DELAY);

    return (bytes_written1) > 0;
}
uint8_t i2s_write_sample_24ch2(uint8_t *sample)
{
    static size_t bytes_written1 = 0;
    static size_t bytes_written2 = 0;
    i2s_write(I2S_NUM, (const uint8_t *)&sample[1], 3, &bytes_written1, portMAX_DELAY);
    i2s_write(I2S_NUM, (const uint8_t *)&sample[5], 3, &bytes_written2, portMAX_DELAY);

    return (bytes_written1 + bytes_written2) > 0;
}
uint8_t i2s_write_sample_32ch2(uint8_t *sample)
{
    static size_t bytes_written1 = 0;
    i2s_write(I2S_NUM, (const uint8_t *)sample, 8, &bytes_written1, portMAX_DELAY);

    return (bytes_written1) > 0;
}
#ifdef SAMPLE_BUFFER_SIZE
uint8_t i2s_write_stereo_buff(float *fl_sample, float *fr_sample, const int buffLen)
{
#ifdef SAMPLE_SIZE_32BIT
    static union t_sample
    {
        uint64_t sample;
        int32_t ch[2];
    } sample_data_u[SAMPLE_BUFFER_SIZE];
#endif
#ifdef SAMPLE_SIZE_24BIT
    static union t_sample
    {
        int32_t ch[2];
        uint8_t sample[8];
    } sample_data_u[SAMPLE_BUFFER_SIZE];
#endif
#ifdef SAMPLE_SIZE_16BIT
    static union t_sample
    {
        uint32_t sample;
        int16_t ch[2];
    } sample_data_u[SAMPLE_BUFFER_SIZE];
#endif

    for (int n = 0; n < buffLen; n++)
    {
#ifdef SAMPLE_SIZE_16BIT
        sample_data_u[n].ch[0] = (int16_t)(fr_sample[n] * 16383.0f); /* some bits missing here */
        sample_data_u[n].ch[1] = (int16_t)(fl_sample[n] * 16383.0f);
#endif
#ifdef SAMPLE_SIZE_32BIT
        sample_data_u[n].ch[0] = (int32_t)(fr_sample[n] * 1073741823.0f); /* some bits missing here */
        sample_data_u[n].ch[1] = (int32_t)(fl_sample[n] * 1073741823.0f);
#endif
    }

    static size_t bytes_written = 0;

    i2s_write(I2S_NUM, (const char *)&sample_data_u[0].sample, 4 * buffLen, &bytes_written, portMAX_DELAY);

    return (bytes_written > 0);
}
#endif /* #ifdef SAMPLE_BUFFER_SIZE */
uint8_t i2s_write_stereo(float *fl_sample, float *fr_sample)
{

#ifdef SAMPLE_SIZE_32BIT
    static union t_sample
    {
        uint64_t sample;
        int32_t ch[2];
    } sample_data_u;
#endif
#ifdef SAMPLE_SIZE_24BIT

    static union t_sample
    {
        int32_t ch[2];
        uint8_t sample[8];
    } sample_data_u;
#endif
#ifdef SAMPLE_SIZE_16BIT
    static union t_sample
    {
        uint32_t sample;
        int16_t ch[2];
    } sample_data_u;
#endif

#ifdef SAMPLE_SIZE_16BIT
    sample_data_u.ch[0] = (int16_t)(*fr_sample * 16383.0f); /* some bits missing here */
    sample_data_u.ch[1] = (int16_t)(*fl_sample * 16383.0f);
#endif
#ifdef SAMPLE_SIZE_32BIT
    sample_data_u.ch[0] = (int32_t)(*fr_sample * 1073741823.0f); /* some bits missing here */
    sample_data_u.ch[1] = (int32_t)(*fl_sample * 1073741823.0f);
#endif

    static size_t bytes_written = 0;

#ifdef SAMPLE_SIZE_16BIT
    i2s_write(I2S_NUM, (const uint8_t *)&sample_data_u.sample, 4, &bytes_written, portMAX_DELAY);
#endif
#ifdef SAMPLE_SIZE_24BIT
    i2s_write(I2S_NUM, (const uint8_t *)&sample_data_u.sample, 6, &bytes_written, portMAX_DELAY);
#endif
#ifdef SAMPLE_SIZE_32BIT
    i2s_write(I2S_NUM, (const uint8_t *)&sample_data_u.sample, 8, &bytes_written, portMAX_DELAY);
#endif

    return bytes_written > 0;
}
