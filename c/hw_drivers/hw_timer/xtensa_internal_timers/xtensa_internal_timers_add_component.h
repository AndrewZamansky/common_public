/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "xtensa_internal_timers_api.h"
#include  "src/xtensa_internal_timers.h"


#define	MODULE_NAME						xtensa_internal_timers
#define	MODULE_IOCTL_FUNCTION			xtensa_internal_timers_ioctl
#define MODULE_CONFIG_DATA_STRUCT_TYPE	struct xtensa_internal_timers_instance_t
#define MODULE_RUNTIME_DATA_STRUCT_TYPE	struct xtensa_internal_timers_runtime_t


#ifdef DT_DEV_MODULE

	#ifndef XTENSA_INTERNAL_TIMERS_DT_CLOCK_PDEV
		#error "XTENSA_INTERNAL_TIMERS_DT_CLOCK_PDEV should be defined"
	#endif

	#ifndef XTENSA_INTERNAL_TIMERS_DT_INITIAL_RATE
		#error "XTENSA_INTERNAL_TIMERS_DT_INITIAL_RATE should be defined"
	#endif

	#ifndef XTENSA_INTERNAL_TIMERS_DT_INDEX
		#error "XTENSA_INTERNAL_TIMERS_DT_INDEX should be defined"
	#endif

	#ifndef XTENSA_INTERNAL_TIMERS_DT_CALLBACK_FUNC
		#define	__XTENSA_INTERNAL_TIMERS_CALLBACK_FUNC     NULL
	#else
		extern timer_callback_func_t XTENSA_INTERNAL_TIMERS_DT_CALLBACK_FUNC;
		#define __XTENSA_INTERNAL_TIMERS_CALLBACK_FUNC	XTENSA_INTERNAL_TIMERS_DT_CALLBACK_FUNC
	#endif

	EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(XTENSA_INTERNAL_TIMERS_DT_CLOCK_PDEV) ;
	#define STATIC_DEV_DATA_STRUCT 	{								\
		XTENSA_INTERNAL_TIMERS_DT_INITIAL_RATE ,							\
		P_TO_STATIC_DEVICE_INST(XTENSA_INTERNAL_TIMERS_DT_CLOCK_PDEV) ,	\
		__XTENSA_INTERNAL_TIMERS_CALLBACK_FUNC ,							\
		XTENSA_INTERNAL_TIMERS_DT_INDEX  }
#endif


/**
 *  device specific defines should be undefined
 *  after calling #include "add_static_dev.h"
 */
#include "add_component.h"

#undef XTENSA_INTERNAL_TIMERS_DT_INITIAL_RATE
#undef XTENSA_INTERNAL_TIMERS_DT_INDEX
#undef XTENSA_INTERNAL_TIMERS_DT_CALLBACK
#undef XTENSA_INTERNAL_TIMERS_DT_CALLBACK_FUNC
#undef __XTENSA_INTERNAL_TIMERS_CALLBACK_FUNC
