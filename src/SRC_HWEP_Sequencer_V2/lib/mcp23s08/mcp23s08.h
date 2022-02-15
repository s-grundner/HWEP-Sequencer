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

// https://ww1.microchip.com/downloads/en/DeviceDoc/MCP23008-MCP23S08-Data-Sheet-20001919F.pdf

// ------------------------------------------------------------
// Register Addresses
// ------------------------------------------------------------

typedef enum
{
	IODIR = 0x00,
	IPOL = 0x01,
	GPINTEN = 0x02,
	DEFVAL = 0x03,
	INTCON = 0x04,
	IOCON = 0x05,
	GPPU = 0x06,
	INTF = 0x07,
	INTCAP = 0x08, //(Read-only)
	GPIO_R = 0x09,
	OLAT = 0x0A,
} mcp23s08_reg_adr;

// ------------------------------------------------------------
// Hardware Addresses
// ------------------------------------------------------------

typedef enum
{
	HW_ADR_0 = 0b00,
	HW_ADR_1 = 0b01,
	HW_ADR_2 = 0b10,
	HW_ADR_3 = 0b11,
} mcp23s08_hw_adr;

// ------------------------------------------------------------
// Structs
// ------------------------------------------------------------

typedef struct
{
	spi_host_device_t host;
	gpio_num_t cs_io;
	gpio_num_t miso_io;
	gpio_num_t mosi_io;
} mcp23s08_config_t;

struct mcp23s08_context_t
{
	mcp23s08_config_t cfg;
	spi_device_handle_t spi;
	SemaphoreHandle_t ready_sem;
};

typedef struct mcp23s08_context_t mcp23s08_context_t;
typedef struct mcp23s08_context_t *mcp23s08_handle_t;

// ------------------------------------------------------------
// Transaction specifications
// ------------------------------------------------------------

#define F_SCK (SPI_MASTER_FREQ_10M)

// ------------------------------------------------------------
// (Public) Functions
// ------------------------------------------------------------

esp_err_t mcp23s08_init(mcp23s08_handle_t *out_handle, const mcp23s08_config_t *cfg);
esp_err_t mcp23s08_write(mcp23s08_handle_t out_handle, mcp23s08_hw_adr hw_adr, mcp23s08_reg_adr reg_adr, const uint8_t data);
esp_err_t mcp23s08_read(mcp23s08_handle_t out_handle, mcp23s08_hw_adr hw_adr, mcp23s08_reg_adr reg_adr, uint8_t *data);
