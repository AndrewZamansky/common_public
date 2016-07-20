/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "cortexM_systick_api.h"

#ifndef CORTEXM_SYSTICK_DT_INITIAL_RATE
	#error "CORTEXM_SYSTICK_DT_INITIAL_RATE should be defined"
#endif

#ifndef CORTEXM_SYSTICK_DT_MODE
	#error "CORTEXM_SYSTICK_DT_MODE should be defined"
#endif

#ifndef CORTEXM_SYSTICK_DT_CALLBACK
	#define	CORTEXM_SYSTICK_DT_CALLBACK     NULL
#endif


#include "src/cortexM_systick.h"
uint8_t cortexM_systick_ioctl( void * const aHandle ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2);

#define STATIC_DEV_DATA_STRUCT_TYPE	CORTEXM_SYSTICK_Instance_t
#define STATIC_DEV_DATA_STRUCT 	{									\
								CORTEXM_SYSTICK_DT_INITIAL_RATE ,	\
								CORTEXM_SYSTICK_DT_CALLBACK ,		\
								CORTEXM_SYSTICK_DT_MODE  }

#define	STATIC_DEV_IOCTL_FUNCTION	cortexM_systick_ioctl
#include "add_static_dev.h"

#undef CORTEXM_SYSTICK_DT_INITIAL_RATE
#undef CORTEXM_SYSTICK_DT_MODE
#undef CORTEXM_SYSTICK_DT_CALLBACK

