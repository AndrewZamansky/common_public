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

#include "src/_heartbeat_prerequirements_check.h"


/***************   typedefs    *******************/
typedef void (*heartbeat_callback_func_t)(void)  ;

typedef struct {
	pdev_descriptor_const 	heartbeat_callback_dev;
	pdev_descriptor_const   callibration_timer;
} heartbeat_instance_t;

#endif /* */
