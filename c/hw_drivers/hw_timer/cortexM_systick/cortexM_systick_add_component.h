/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "cortexM_systick_api.h"
#include  "src/cortexM_systick.h"


#define	MODULE_NAME						cortexM_systick
#define	MODULE_IOCTL_FUNCTION			cortexM_systick_ioctl
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

	#ifndef CORTEXM_SYSTICK_DT_CALLBACK_FUNC
		#define	__CORTEXM_SYSTICK_CALLBACK_FUNC     NULL
	#else
		extern timer_callback_func_t CORTEXM_SYSTICK_DT_CALLBACK_FUNC;
		#define __CORTEXM_SYSTICK_CALLBACK_FUNC	CORTEXM_SYSTICK_DT_CALLBACK_FUNC
	#endif

	EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(CORTEXM_SYSTICK_DT_CLOCK_PDEV) ;
	#define STATIC_DEV_DATA_STRUCT 	{								\
		CORTEXM_SYSTICK_DT_INITIAL_RATE ,							\
		P_TO_STATIC_DEVICE_INST(CORTEXM_SYSTICK_DT_CLOCK_PDEV) ,	\
		__CORTEXM_SYSTICK_CALLBACK_FUNC ,							\
		CORTEXM_SYSTICK_DT_MODE  }
#endif


/**
 *  device specific defines should be undefined
 *  after calling #include "add_static_dev.h"
 */
#include "add_component.h"

#undef CORTEXM_SYSTICK_DT_INITIAL_RATE
#undef CORTEXM_SYSTICK_DT_MODE
#undef CORTEXM_SYSTICK_DT_CALLBACK
#undef CORTEXM_SYSTICK_DT_CALLBACK_FUNC
#undef __CORTEXM_SYSTICK_CALLBACK_FUNC
