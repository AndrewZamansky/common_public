/*
 * file : heartbeat.h
 *
 *
 */

#ifndef _HEARTBEAT_H
#define _HEARTBEAT_H



/***************   typedefs    *******************/
typedef void (*heartbeat_callback_func_t)(void)  ;

struct heartbeat_instance_t {
	struct dev_desc_t * 	heartbeat_callback_dev;
	struct dev_desc_t *   callibration_timer;
};

#endif /* */
