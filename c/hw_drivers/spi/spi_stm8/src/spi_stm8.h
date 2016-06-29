/*
 *
 * spi_stm8.h
 *
 *
 *
 *
 *
 */

#ifndef _SPI_STM8_H_
#define _SPI_STM8_H_

/********  includes *********************/

#include "src/_spi_stm8_prerequirements_check.h" // should be after {spi_stm8_config.h,dev_managment_api.h}

/********  types  *********************/

typedef struct {
	uint8_t dummy;
} SPI_STM8_Instance_t;

#endif
