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
	struct dev_desc_t * 	heartbeat_callback_dev;
	struct dev_desc_t *   callibration_timer;
} heartbeat_instance_t;

#endif /* */
