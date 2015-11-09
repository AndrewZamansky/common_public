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

#include "I2S_nuc505_config.h"
#include "dev_managment_api.h" // for device manager defines and typedefs
#include "src/_I2S_nuc505_prerequirements_check.h" // should be after {I2S_nuc505_config.h,dev_managment_api.h}

/********  types  *********************/

typedef struct {
	pdev_descriptor_const   callback_dev;
} I2S_NUC505_Instance_t;

#endif
