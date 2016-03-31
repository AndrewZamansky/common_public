/*
 *
 * semihosting.h
 *
 *
 *
 *
 *
 */

#ifndef _SEMIHOSTING_H_
#define _SEMIHOSTING_H_

/********  includes *********************/

#include "arm_semihosting_config.h"
#include "dev_managment_api.h" // for device manager defines and typedefs
#include "src/_semihosting_prerequirements_check.h" // should be after {semihosting_config.h,dev_managment_api.h}

/********  types  *********************/

typedef struct {
	pdev_descriptor_const   callback_dev;
} SEMIHOSTING_Instance_t;

#endif
