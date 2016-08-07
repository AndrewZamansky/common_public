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

#include "src/_spi_stm8_prerequirements_check.h" // should be after {spi_stm8_config.h,dev_management_api.h}

/********  types  *********************/

typedef struct {
	uint8_t dummy;
}spi_stm8_instance_t;

#endif
