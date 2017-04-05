/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include "heartbeat_api.h"
#include "src/heartbeat.h"

extern uint8_t heartbeat_ioctl( struct dev_desc_t *adev ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2);
extern uint8_t heartbeat_init( struct dev_desc_t *aDevDescriptor)	;

#define	MODULE_NAME				heartbeat
#define	MODULE_INIT_FUNCTION	heartbeat_init
#define	MODULE_IOCTL_FUNCTION	heartbeat_ioctl
#define MODULE_CONFIG_DATA_STRUCT_TYPE	heartbeat_instance_t

#ifdef DT_DEV_MODULE

	#ifndef HEARTBEAT_DT_CALLBACK_PDEV
		#error "HEARTBEAT_DT_CALLBACK_PDEV should be defined"
	#endif

	#ifndef HEARTBEAT_DT_OS_TIMER_PDEV
		#error "HEARTBEAT_DT_OS_TIMER_PDEV should be defined"
	#endif

	EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(HEARTBEAT_DT_OS_TIMER_PDEV) ;
	EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(HEARTBEAT_DT_CALLBACK_PDEV) ;

	#define STATIC_DEV_DATA_STRUCT									\
		{															\
			P_TO_STATIC_DEVICE_INST(HEARTBEAT_DT_CALLBACK_PDEV),	\
			P_TO_STATIC_DEVICE_INST(HEARTBEAT_DT_OS_TIMER_PDEV) ,	\
		}

#endif

#include "add_component.h"

/* device specific defines should be undefined after calling #include "add_static_dev.h" */
#undef HEARTBEAT_DT_CALLBACK_FUNC
#undef HEARTBEAT_DT_OS_TIMER_PDEV
