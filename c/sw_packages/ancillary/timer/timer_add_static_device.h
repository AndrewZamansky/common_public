/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "timer_api.h"


#ifndef TIMER_DT_HW_TIMER_PDEV
#error "TIMER_DT_HW_TIMER_PDEV should be defined"
#endif



extern uint8_t timer_ioctl( void * const aHandle ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2);

#include "src/timer.h"

EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(TIMER_DT_HW_TIMER_PDEV) ;
#define MODULE_DATA_STRUCT_TYPE	timer_instance_t
#define STATIC_DEV_DATA_STRUCT									\
	{															\
		P_TO_STATIC_DEVICE_INST(TIMER_DT_HW_TIMER_PDEV) ,	\
			0 ,												\
			0												\
	}

#define	MODULE_IOCTL_FUNCTION	timer_ioctl
#include "add_static_dev.h"


#undef TIMER_DT_HW_TIMER_PDEV

