#ifndef __add_module_h__
#define __add_module_h__

#include "dev_management_api.h"
#include "arch.h"

#if (defined(CONFIG_DYNAMIC_DEVICE_TREE) ||  \
		defined(CONFIG_USE_RUNTIME_DEVICE_CONFIGURATION_BY_PARAMETER_NAMES) || \
		(CONFIG_MAX_NUM_OF_DYNAMIC_DEVICES>0))

	#ifndef MODULE_NAME
		#error "MODULE_NAME should be defined"
	#endif

	#ifndef	MODULE_CONFIG_DATA_STRUCT_TYPE
		#define MODULE_CONFIG_DATA_STRUCT_TYPE			uint8_t
	#endif

	#ifndef	MODULE_RUNTIME_DATA_STRUCT_TYPE
		#define MODULE_RUNTIME_DATA_STRUCT_TYPE			uint8_t
	#endif

	#define MODULE_INST2(module)	module_inst_##module
	#define MODULE_INST(module)		MODULE_INST2(module)

	#define MODULE_PARAMS_ARR2(module)		params_array_##module
	#define MODULE_PARAMS_ARR(module)		MODULE_PARAMS_ARR2(module)

	#ifdef CONFIG_USE_RUNTIME_DEVICE_CONFIGURATION_BY_PARAMETER_NAMES
		#ifdef MODULE_CONFIGURABLE_PARAMS_ARRAY
			struct dev_param_t const MODULE_PARAMS_ARR(MODULE_NAME)[] =
									MODULE_CONFIGURABLE_PARAMS_ARRAY;
			#define	POINTER_TO_CONFIGURABLE_PARAMS_ARRAY	\
											MODULE_PARAMS_ARR(MODULE_NAME)
			#define	SIZE_OF_POINTER_TO_CONFIGURABLE_PARAMS_ARRAY	\
			  sizeof(MODULE_PARAMS_ARR(MODULE_NAME))/sizeof(struct dev_param_t)
		#else
			#define	POINTER_TO_CONFIGURABLE_PARAMS_ARRAY			NULL
			#define SIZE_OF_POINTER_TO_CONFIGURABLE_PARAMS_ARRAY	0
		#endif
	#endif


	MODULES_PLACEMENT struct included_module_t MODULE_INST(MODULE_NAME) =
		{
			STRINGIFY(MODULE_NAME)	,
			MODULE_IOCTL_FUNCTION 	,
			MODULE_PWRITE_FUNCTION 	,
			MODULE_PREAD_FUNCTION 	,
			MODULE_CALLBACK_FUNCTION,
	#ifdef CONFIG_USE_RUNTIME_DEVICE_CONFIGURATION_BY_PARAMETER_NAMES
			POINTER_TO_CONFIGURABLE_PARAMS_ARRAY,
			SIZE_OF_POINTER_TO_CONFIGURABLE_PARAMS_ARRAY ,
	#endif
			sizeof(MODULE_CONFIG_DATA_STRUCT_TYPE),
			sizeof(MODULE_RUNTIME_DATA_STRUCT_TYPE),
			(void*)MODULE_MAGIC_NUMBER
		};

#endif

#endif/*__add_module_h__*/
