/**
 * @file 	mcp23s08.h
 * @author 	@s-grundner
 * @brief 	Library for the mcp23s08 SPI IO-Expander
 * @version 0.1
 * @date 	2022-05-06
 * 
 * @copyright Copyright (c) 2022
 * https://ww1.microchip.com/downloads/en/DeviceDoc/MCP23008-MCP23S08-Data-Sheet-20001919F.pdf
 */

#ifndef MCP23S08_H_
#define MCP23S08_H_

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

#include "mcp23s08_io.h"

// ------------------------------------------------------------
// Structs
// ------------------------------------------------------------


typedef void (*mcp_intr_cb_fn_t)(void *args);

typedef struct
{
	spi_host_device_t host;
	gpio_num_t cs_io;
	gpio_num_t miso_io;
	gpio_num_t mosi_io;
	gpio_num_t intr_io; // -1 if interrupts ar not used
	mcp_intr_cb_fn_t mcp_callback;
	void* mcp_intr_args;
} mcp23s08_config_t;

typedef struct mcp23s08_context_t* mcp23s08_handle_t;

// ------------------------------------------------------------
// Transaction specifications
// ------------------------------------------------------------

#define F_SCK_MCP (SPI_MASTER_FREQ_10M)

// ------------------------------------------------------------
// (Public) Functions
// ------------------------------------------------------------

/**
 * @brief initialises the IO expander
 * 
 * @param out_handle provide Extern Handle to store bus informations
 * @param cfg externally configurated bus settings
 * @return esp_err_t For ESP_ERROR_CHECK() macro
 */
esp_err_t mcp23s08_init(mcp23s08_handle_t *out_handle, const mcp23s08_config_t *cfg);

/**
 * @brief Writes to a Register
 * 
 * @param handle Bus information handle
 * @param hw_adr Hardware address for up tp four IO Expander (IOCON.HAEN = 1 required)
 * @param reg_adr Address of the register (see mcp23s08_io.h or Datasheet)
 * @param data Data to write to the register
 * @return esp_err_t For ESP_ERROR_CHECK() macro
 */
esp_err_t mcp23s08_write(mcp23s08_handle_t handle, mcp23s08_hw_adr hw_adr, mcp23s08_reg_adr reg_adr, const uint8_t data);

/**
 * @brief Reads a Register
 * 
 * @param handle Bus inforamtion handle
 * @param hw_adr Hardware address for up tp four IO Expander (IOCON.HAEN = 1 required)
 * @param reg_adr Address of the register (see mcp23s08_io.h or Datasheet)
 * @param data Data pointer to store the read register data in
 * @return esp_err_t For ESP_ERROR_CHECK() macro
 */
esp_err_t mcp23s08_read(mcp23s08_handle_t handle, mcp23s08_hw_adr hw_adr, mcp23s08_reg_adr reg_adr, uint8_t *data);

/**
 * @brief Reads INTCAP register to give clearance to new interrupt signals
 * 
 * @param handle Bus information handle
 * @param hw_adr Hardware address for up tp four IO Expander (IOCON.HAEN = 1 required)
 * @return esp_err_t 
 */
esp_err_t mcp23s08_dump_intr(mcp23s08_handle_t handle, mcp23s08_hw_adr hw_adr);
#endif // MCP23S08_H_