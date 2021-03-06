
#ifndef _SPI_STM32f10X_API_H_
#define _SPI_STM32f10X_API_H_

#include "spi_stm32f10x_config.h"
#include "dev_management_api.h"

uint8_t  spi_stm32f10x_api_init_dev_descriptor(struct dev_desc_t *aDevDescriptor);

#include "src/spi_stm32f10x_static_dev_macros.h"

#define SPI_STM32F10X_API_CREATE_STATIC_DEV(dev,dev_name  ) \
			__SPI_STM32F10X_API_CREATE_STATIC_DEV(dev,dev_name  )

#endif
