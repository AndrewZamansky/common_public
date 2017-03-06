/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "timer_wrapper_api.h"
#include "src/timer_wrapper.h"

extern uint8_t timer_wrapper_ioctl( pdev_descriptor_t apdev ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2);

#define	MODULE_NAME					timer_wrapper
#define	MODULE_IOCTL_FUNCTION		timer_wrapper_ioctl
#define MODULE_CONFIG_DATA_STRUCT_TYPE		timer_wrapper_instance_t
#define MODULE_RUNTIME_DATA_STRUCT_TYPE		timer_wrapper_runtime_instance_t

#ifdef DT_DEV_MODULE

	#ifndef TIMER_WRAPPER_DT_HW_TIMER_PDEV
		#error "TIMER_WRAPPER_DT_HW_TIMER_PDEV should be defined"
	#endif

	EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(TIMER_WRAPPER_DT_HW_TIMER_PDEV) ;
	#define STATIC_DEV_DATA_STRUCT									\
		{															\
			P_TO_STATIC_DEVICE_INST(TIMER_WRAPPER_DT_HW_TIMER_PDEV) 		\
		}
#endif

#include "add_component.h"

/* device specific defines should be undefined after calling #include "add_component.h" */
#undef TIMER_DT_HW_TIMER_PDEV
