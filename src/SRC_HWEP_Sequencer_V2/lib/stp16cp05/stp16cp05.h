#ifndef STP16CP05_H_
#define STP16CP05_H_

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


// ------------------------------------------------------------
// Structs
// ------------------------------------------------------------

typedef struct
{
	spi_host_device_t host;
	gpio_num_t cs_io;
	gpio_num_t miso_io;
	gpio_num_t mosi_io;
} stp16cp05_config_t;

typedef struct stp16cp05_context_t* stp16cp05_handle_t;

// ------------------------------------------------------------
// Transaction specifications
// ------------------------------------------------------------

#define F_SCK_STP (SPI_MASTER_FREQ_20M)

// ------------------------------------------------------------
// (Public) Functions
// ------------------------------------------------------------

esp_err_t stp16cp05_init(stp16cp05_handle_t *out_handle, const stp16cp05_config_t *cfg);
esp_err_t stp16cp05_write(stp16cp05_handle_t handle, const uint8_t data_top, const uint8_t data_bot);

#endif // STP16CP05_H_