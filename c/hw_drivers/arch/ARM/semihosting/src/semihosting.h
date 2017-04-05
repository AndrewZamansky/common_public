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
	struct dev_desc_t *   callback_dev;
} SEMIHOSTING_Instance_t;

#endif
