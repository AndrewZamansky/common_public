#ifndef __add_module_h__
#define __add_module_h__

#include "dev_management_api.h"

#if defined(CONFIG_DYNAMIC_DEVICE_TREE) || (CONFIG_MAX_NUM_OF_DYNAMIC_DEVICES > 0)

	#ifndef MODULE_NAME
		#error "MODULE_NAME should be defined"
	#endif

	#ifndef	MODULE_IOCTL_FUNCTION
		#define MODULE_IOCTL_FUNCTION			DEV_API_dummy_ioctl_func
	#endif


	#ifndef	MODULE_PWRITE_FUNCTION
		#define MODULE_PWRITE_FUNCTION			DEV_API_dummy_pwrite_func
	#endif

	#ifndef	MODULE_PREAD_FUNCTION
		#define MODULE_PREAD_FUNCTION			DEV_API_dummy_pread_func
	#endif

	#ifndef	MODULE_CALLBACK_FUNCTION
		#define MODULE_CALLBACK_FUNCTION		DEV_API_dummy_callback_func
	#endif

	#ifndef	MODULE_DATA_STRUCT_TYPE
		#define MODULE_DATA_STRUCT_TYPE			uint8_t
	#endif

	#define MODULE_INST2(module)	module_inst_##module
	#define MODULE_INST(module)		MODULE_INST2(module)

	#define MODULE_PARAMS_ARR2(module)		params_array_##module
	#define MODULE_PARAMS_ARR(module)		MODULE_PARAMS_ARR2(module)

	#ifdef CONFIG_USE_SHELL_FOR_DEVICE_CONFIGURATION
		#ifdef MODULE_CONFIGURABLE_PARAMS_ARRAY
			const dev_param_t MODULE_PARAMS_ARR(MODULE_NAME)[] = MODULE_CONFIGURABLE_PARAMS_ARRAY;
			#define	POINTER_TO_CONFIGURABLE_PARAMS_ARRAY	&MODULE_PARAMS_ARR(MODULE_NAME)
		#else
			#define	POINTER_TO_CONFIGURABLE_PARAMS_ARRAY	NULL
		#endif
	#endif


	MODULES_PLACEMENT included_module_t MODULE_INST(MODULE_NAME) =
		{
			STRINGIFY(MODULE_NAME)	,
			MODULE_IOCTL_FUNCTION 	,
			MODULE_PWRITE_FUNCTION 	,
			MODULE_PREAD_FUNCTION 	,
			MODULE_CALLBACK_FUNCTION,
	#ifdef CONFIG_USE_SHELL_FOR_DEVICE_CONFIGURATION
			POINTER_TO_CONFIGURABLE_PARAMS_ARRAY,
	#endif
			sizeof(MODULE_DATA_STRUCT_TYPE)
		};

#endif

#endif/*__add_module_h__*/
