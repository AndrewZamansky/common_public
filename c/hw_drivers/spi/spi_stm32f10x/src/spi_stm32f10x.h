/*
 *
 * spi_stm32f10x.h
 *
 *
 *
 *
 *
 */

#ifndef _SPI_STM32F10X_H_
#define _SPI_STM32F10X_H_

/********  includes *********************/

#include "spi_stm32f10x_config.h"
#include "dev_managment_api.h" // for device manager defines and typedefs
#include "src/_spi_stm32f10x_prerequirements_check.h" // should be after {spi_stm32f10x_config.h,dev_managment_api.h}

/********  types  *********************/

typedef struct {
	uint8_t dummy;
} SPI_STM32F10X_Instance_t;

#endif
