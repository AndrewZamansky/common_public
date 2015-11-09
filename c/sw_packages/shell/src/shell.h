/*
 * file : SW_UART_WRAPPER.h
 *
 *
 *
 *
 *
 *
 */

#ifndef _SHELL_H
#define _SHELL_H

#include "shell_config.h"
#include "dev_managment_api.h" // for device manager defines and typedefs
#include "src/_shell_prerequirements_check.h" // should be after {shell_config.h,dev_managment_api.h}


/***************   typedefs    *******************/

typedef struct {
	pdev_descriptor_const server_dev;
	pdev_descriptor_const this_dev;
	uint32_t lastTestedBytePos;
	uint8_t lastEOLchar;
} shell_instance_t;

#endif /* */
