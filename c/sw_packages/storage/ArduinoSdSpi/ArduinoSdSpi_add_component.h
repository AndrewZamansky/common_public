/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include "ArduinoSdSpi_api.h"
#include "src/ArduinoSdSpi.h"


#define	MODULE_NAME                       ArduinoSdSpi_dev
#define	MODULE_PWRITE_FUNCTION            ArduinoSdSpi_pwrite
#define	MODULE_PREAD_FUNCTION             ArduinoSdSpi_pread
#define	MODULE_IOCTL_FUNCTION             ArduinoSdSpi_ioctl
#define MODULE_CONFIG_DATA_STRUCT_TYPE    struct arduino_sd_spi_cfg_t
#define MODULE_RUNTIME_DATA_STRUCT_TYPE   struct arduino_sd_spi_runtime_t


#ifdef DT_DEV_MODULE

	#ifndef ARDUINO_SD_SPI_DT_SPI_DEV
		#error "ARDUINO_SD_SPI_DT_SPI_DEV should be defined"
	#endif
	#define _POINTER_TO_SPI_DEV        \
			P_TO_STATIC_DEVICE_INST(ARDUINO_SD_SPI_DT_SPI_DEV)

	#ifndef ARDUINO_SD_SPI_DT_CLK_FREQ
		#error "ARDUINO_SD_SPI_DT_CLK_FREQ should be defined"
	#endif

	#define STATIC_DEV_DATA_STRUCT      \
		{                               \
			_POINTER_TO_SPI_DEV,        \
			ARDUINO_SD_SPI_DT_CLK_FREQ  \
		}
#endif

#include "add_component.h"

#undef  ARDUINO_SD_SPI_DT_SPI_DEV
#undef  _POINTER_TO_SPI_DEV
