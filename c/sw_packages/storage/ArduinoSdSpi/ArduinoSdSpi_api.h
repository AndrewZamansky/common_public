
#ifndef _ArduinoSdSpi_API_H_
#define _ArduinoSdSpi_API_H_


#include "dev_management_api.h"


struct arduino_sd_spi_cfg_t{
	struct dev_desc_t *spi_dev;
	uint32_t clk_freq;
};
SET_CONFIG_TYPE(ArduinoSdSpi, struct arduino_sd_spi_cfg_t);

#endif
