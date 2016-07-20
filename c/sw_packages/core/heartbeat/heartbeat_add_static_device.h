/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "heartbeat_api.h"


#ifndef HEARTBEAT_DT_CALLBACK_PDEV
#error "HEARTBEAT_DT_CALLBACK_PDEV should be defined"
#endif

#ifndef HEARTBEAT_DT_OS_TIMER_PDEV
#error "HEARTBEAT_DT_OS_TIMER_PDEV should be defined"
#endif


extern uint8_t heartbeat_ioctl( void * const aHandle ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2);

#include "src/heartbeat.h"


EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(HEARTBEAT_DT_OS_TIMER_PDEV) ;
EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(HEARTBEAT_DT_CALLBACK_PDEV) ;

#define STATIC_DEV_DATA_STRUCT_TYPE	heartbeat_instance_t
#define STATIC_DEV_DATA_STRUCT									\
	{															\
		P_TO_STATIC_DEVICE_INST(HEARTBEAT_DT_CALLBACK_PDEV),	\
		P_TO_STATIC_DEVICE_INST(HEARTBEAT_DT_OS_TIMER_PDEV) ,	\
	}

#define	STATIC_DEV_IOCTL_FUNCTION	heartbeat_ioctl
#include "add_static_dev.h"


#undef HEARTBEAT_DT_CALLBACK_FUNC
#undef HEARTBEAT_DT_OS_TIMER_PDEV
