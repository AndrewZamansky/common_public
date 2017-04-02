/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "cortexM_systick_api.h"
#include "src/cortexM_systick.h"

uint8_t cortexM_systick_ioctl( pdev_descriptor_t apdev ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2);

#define	MODULE_NAME				cortexM_systick
#define	MODULE_IOCTL_FUNCTION	cortexM_systick_ioctl
#define MODULE_CONFIG_DATA_STRUCT_TYPE	cortexM_systick_instance_t

#ifdef DT_DEV_MODULE

	#ifndef CORTEXM_SYSTICK_DT_CLOCK_PDEV
		#error "CORTEXM_SYSTICK_DT_CLOCK_PDEV should be defined"
	#endif

	#ifndef CORTEXM_SYSTICK_DT_INITIAL_RATE
		#error "CORTEXM_SYSTICK_DT_INITIAL_RATE should be defined"
	#endif

	#ifndef CORTEXM_SYSTICK_DT_MODE
		#error "CORTEXM_SYSTICK_DT_MODE should be defined"
	#endif

	#ifndef CORTEXM_SYSTICK_DT_CALLBACK
		#define	CORTEXM_SYSTICK_DT_CALLBACK     NULL
	#endif

	EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(CORTEXM_SYSTICK_DT_CLOCK_PDEV) ;
	#define STATIC_DEV_DATA_STRUCT 	{	\
		CORTEXM_SYSTICK_DT_INITIAL_RATE ,	\
		P_TO_STATIC_DEVICE_INST(CORTEXM_SYSTICK_DT_CLOCK_PDEV) ,	\
		CORTEXM_SYSTICK_DT_CALLBACK ,		\
		CORTEXM_SYSTICK_DT_MODE  }
#endif

#include "add_component.h"

/* device specific defines should be undefined after calling #include "add_component.h" */
#undef CORTEXM_SYSTICK_DT_INITIAL_RATE
#undef CORTEXM_SYSTICK_DT_MODE
#undef CORTEXM_SYSTICK_DT_CALLBACK

