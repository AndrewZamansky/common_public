/*
 * file : SW_TIMER_WRAPPER.h
 *
 *
 *
 *
 *
 *
 */

#ifndef _SW_TIMER_WRAPPER_H
#define _SW_TIMER_WRAPPER_H

#include "sw_timer_wrapper_config.h"
#include "dev_management_api.h" // for device manager defines and typedefs
#include "src/_sw_timer_wrapper_prerequirements_check.h" // should be after {sw_timer_wrapper_config.h,dev_management_api.h}


/***************   typedefs    *******************/



typedef struct {

	pdev_descriptor_const   server_dev;

	uint32 timeout ;
	uint8 running ;
} SW_TIMER_WRAPPER_Instance_t;




#endif /* */
