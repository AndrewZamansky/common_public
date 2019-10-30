/*
 * file : ArduinoSdSpi.h
 *
 *
 */

#ifndef _ArduinoSdSpi_H
#define _ArduinoSdSpi_H

#include "dev_management_api.h"

struct arduino_sd_spi_runtime_t {
	void *sd_spi_inst;
	void *SdSpiAltDriver_obj;
	uint8_t num_dev_inst;
};
SET_RUNTIME_DATA_TYPE(ArduinoSdSpi, struct arduino_sd_spi_runtime_t);

#endif
