#ifndef _add_static_device_step1
#define _add_static_device_step1

#include  "heartbeat_api.h"

#undef _add_static_device_step2

#undef HEARTBEAT_DT_DEV_NAME
#undef HEARTBEAT_DT_CALLBACK_FUNC
#undef HEARTBEAT_DT_OS_TIMER_PDEV


#elif !defined(_add_static_device_step2)
#define _add_static_device_step2

#undef _add_static_device_step1

#ifndef HEARTBEAT_DT_DEV_NAME
#error "HEARTBEAT_DT_DEV_NAME should be defined"
#endif

#ifndef HEARTBEAT_DT_CALLBACK_FUNC
#error "HEARTBEAT_DT_CALLBACK_FUNC should be defined"
#endif

#ifndef HEARTBEAT_DT_OS_TIMER_PDEV
#error "HEARTBEAT_DT_OS_TIMER_PDEV should be defined"
#endif


extern uint8_t heartbeat_ioctl( void * const aHandle ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2);

#include "src/heartbeat.h"

EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(HEARTBEAT_DT_OS_TIMER_PDEV) ;
heartbeat_instance_t STATIC_DEVICE_INNER_INST(HEARTBEAT_DT_DEV_NAME) =
	{	HEARTBEAT_DT_CALLBACK_FUNC ,
		P_TO_STATIC_DEVICE_INST(HEARTBEAT_DT_OS_TIMER_PDEV)
	};

CREATE_STATIC_DEVICE(HEARTBEAT_DT_DEV_NAME , &STATIC_DEVICE_INNER_INST(HEARTBEAT_DT_DEV_NAME) ,
		heartbeat_ioctl ,  DEV_API_dummy_pwrite_func ,
		DEV_API_dummy_pread_func , DEV_API_dummy_callback_func);



#undef CURRENT_DEV

#endif
