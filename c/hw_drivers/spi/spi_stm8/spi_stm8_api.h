
#ifndef _SPI_STM8_API_H_
#define _SPI_STM8_API_H_

#include "spi_stm8_config.h"
#include "dev_managment_api.h" // for device manager defines and typedefs
#include "src/_spi_stm8_prerequirements_check.h" // should be after {spi_stm8_config.h,dev_managment_api.h}

/*****************  defines  **************/


/**********  define API  types ************/



/**********  define API  functions  ************/

uint8_t  spi_stm8_api_init_dev_descriptor(pdev_descriptor aDevDescriptor);


#include "src/spi_stm8_static_dev_macros.h"

#define SPI_STM8_API_CREATE_STATIC_DEV(dev,dev_name  ) \
			__SPI_STM8_API_CREATE_STATIC_DEV(dev,dev_name  )


#endif
