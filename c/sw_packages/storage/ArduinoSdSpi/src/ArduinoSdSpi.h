/*
 * file : ArduinoSdSpi.h
 *
 *
 */

#ifndef _ArduinoSdSpi_H
#define _ArduinoSdSpi_H

#include "dev_management_api.h"


/***************   typedefs    *******************/

struct arduino_sd_spi_cfg_t{
	struct dev_desc_t *spi_dev;
	uint32_t clk_freq;
};

struct arduino_sd_spi_runtime_t {
	void *sd_spi_inst;
	void *SdSpiAltDriver_obj;
	uint8_t num_dev_inst;
};


#endif
