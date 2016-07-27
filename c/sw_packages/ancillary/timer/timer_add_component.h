/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "timer_api.h"
#include "src/timer.h"

extern uint8_t timer_ioctl( void * const aHandle ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2);

#define	MODULE_NAME					timer
#define	MODULE_IOCTL_FUNCTION		timer_ioctl
#define MODULE_DATA_STRUCT_TYPE		timer_instance_t

#ifdef DT_DEV_MODULE

	#ifndef TIMER_DT_HW_TIMER_PDEV
	#error "TIMER_DT_HW_TIMER_PDEV should be defined"
	#endif

	EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(TIMER_DT_HW_TIMER_PDEV) ;
	#define STATIC_DEV_DATA_STRUCT									\
		{															\
			P_TO_STATIC_DEVICE_INST(TIMER_DT_HW_TIMER_PDEV) ,	\
				0 ,												\
				0												\
		}
#endif

#include "add_static_dev.h"

/* device specific defines should be undefined after calling #include "add_component.h" */
#undef TIMER_DT_HW_TIMER_PDEV

