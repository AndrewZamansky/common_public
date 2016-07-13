#ifndef _add_static_device_step1
#define _add_static_device_step1

#include  "cortexM_systick_api.h"

#undef _add_static_device_step2

#undef CORTEXM_SYSTICK_DT_DEV_NAME
#undef CORTEXM_SYSTICK_DT_INITIAL_RATE
#undef CORTEXM_SYSTICK_DT_MODE
#undef CORTEXM_SYSTICK_DT_CALLBACK


#elif !defined(_add_static_device_step2)
#define _add_static_device_step2

#undef _add_static_device_step1

#ifndef CORTEXM_SYSTICK_DT_DEV_NAME
#error "CORTEXM_SYSTICK_DT_DEV_NAME should be defined"
#endif

#ifndef CORTEXM_SYSTICK_DT_INITIAL_RATE
#error "CORTEXM_SYSTICK_DT_INITIAL_RATE should be defined"
#endif

#ifndef CORTEXM_SYSTICK_DT_MODE
#error "CORTEXM_SYSTICK_DT_MODE should be defined"
#endif

#ifndef CORTEXM_SYSTICK_DT_CALLBACK
#error "CORTEXM_SYSTICK_DT_CALLBACK should be defined"
#endif


#include "src/cortexM_systick.h"
uint8_t cortexM_systick_ioctl( void * const aHandle ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2);


CORTEXM_SYSTICK_Instance_t STATIC_DEVICE_INNER_INST(CORTEXM_SYSTICK_DT_DEV_NAME) =
	{
		CORTEXM_SYSTICK_DT_INITIAL_RATE ,
		CORTEXM_SYSTICK_DT_CALLBACK ,
		CORTEXM_SYSTICK_DT_MODE  };

CREATE_STATIC_DEVICE(CORTEXM_SYSTICK_DT_DEV_NAME ,
	&STATIC_DEVICE_INNER_INST(CORTEXM_SYSTICK_DT_DEV_NAME)  ,
	cortexM_systick_ioctl ,  DEV_API_dummy_pwrite_func ,
	DEV_API_dummy_pread_func , DEV_API_dummy_callback_func);

#undef CURRENT_DEV

#endif
