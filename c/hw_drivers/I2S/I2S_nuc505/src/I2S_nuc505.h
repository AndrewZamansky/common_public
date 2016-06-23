/*
 *
 * I2S_nuc505.h
 *
 *
 *
 *
 *
 */

#ifndef _I2S_NUC505_H_
#define _I2S_NUC505_H_

/********  includes *********************/

#include "src/_I2S_nuc505_prerequirements_check.h"

/********  types  *********************/

typedef struct
{
	pdev_descriptor_t   callback_dev;
	uint8_t start_flag;
} I2S_NUC505_Instance_t;

#endif
