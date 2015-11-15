/*
 * file : heartbeat.h
 *
 *
 *
 *
 *
 *
 */

#ifndef _HEARTBEAT_H
#define _HEARTBEAT_H

#include "heartbeat_config.h"
#include "dev_managment_api.h" // for device manager defines and typedefs
#include "src/_heartbeat_prerequirements_check.h" // should be after {heartbeat_config.h,dev_managment_api.h}


/***************   typedefs    *******************/
typedef void (*heartbeat_callback_func_t)(void)  ;

typedef struct {
	heartbeat_callback_func_t heartbeat_callback;
	pdev_descriptor_const   callibration_timer;
} heartbeat_instance_t;

#endif /* */
