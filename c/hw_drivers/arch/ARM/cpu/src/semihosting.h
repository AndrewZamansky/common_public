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

#include "src/_semihosting_prerequirements_check.h"

/********  types  *********************/

typedef struct {
	pdev_descriptor_const   callback_dev;
	pdev_descriptor this_dev;
} SEMIHOSTING_Instance_t;

#endif
