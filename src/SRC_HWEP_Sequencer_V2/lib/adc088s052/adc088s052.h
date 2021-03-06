/**
 * @file 	adc088s052.h
 * @author 	@s-grundner
 * @brief 	Libary for the ADC088S052 SPI Analog-Digital-Converter Chip
 * @version 0.1
 * @date 	2022-05-06
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

#include "hal/spi_types.h"
#include "driver/spi_common.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include <unistd.h>
#include <sys/param.h>
#include "sdkconfig.h"

// ------------------------------------------------------------
// Datasheet
// ------------------------------------------------------------

// https://www.ti.com/lit/ds/symlink/adc088s052.pdf?ts=1645043382836&ref_url=https%253A%252F%252Fwww.ti.com%252Fproduct%252FADC088S052

// ------------------------------------------------------------
// Channels
// ------------------------------------------------------------

typedef enum
{
	ADC088S052_CH_0 = 0,
	ADC088S052_CH_1 = 1,
	ADC088S052_CH_2 = 2,
	ADC088S052_CH_3 = 3,
	ADC088S052_CH_4 = 4,
	ADC088S052_CH_5 = 5,
	ADC088S052_CH_6 = 6,
	ADC088S052_CH_7 = 7,
}adc088s052_channel_t;

#define ADC0880S052_CHANNEL_MAX 8

// ------------------------------------------------------------
// Configuration
// ------------------------------------------------------------

typedef struct
{
	spi_host_device_t host;
	gpio_num_t cs_io;
	gpio_num_t miso_io;
	gpio_num_t mosi_io;
} adc088s052_config_t;

typedef struct adc088s052_context_t *adc088s052_handle_t;

// ------------------------------------------------------------
// Transaction specifications
// ------------------------------------------------------------

#define F_SCK_ADC (SPI_MASTER_FREQ_8M)

// ------------------------------------------------------------
// (Public) Functions
// ------------------------------------------------------------

/**
 * @brief Initialises the ADC
 * 
 * @param out_handle 	externally defined handle to pass arguments and bus information
 * @param cfg 			the adc/bus configuration to which the handle stores
 * @return esp_err_t 
 */
esp_err_t adc088s052_init(adc088s052_handle_t *out_handle, const adc088s052_config_t *cfg);

/**
 * @brief 		reads the adc value of any channel via spi and stores it
 * 
 * @param handle	ADC Handler for businformation
 * @param ch 		ADC Channel (0-8)
 * @param data 		data pointer to store the read data in 
 * @return esp_err_t 
 */
esp_err_t adc088s052_get_raw(adc088s052_handle_t handle, adc088s052_channel_t ch, uint8_t *data);

/**
 * @brief Frees the memory of the handle
 * @param handle 
 */
void adc088s052_exit(adc088s052_handle_t handle);