#ifndef __add_module_h__
#define __add_module_h__

#include "dev_management_api.h"
#include "arch.h"

#ifndef MODULE_NAME
	#error "MODULE_NAME should be defined"
#endif


#ifndef	MODULE_IOCTL_FUNCTION
	#define MODULE_IOCTL_FUNCTION			DEV_API_dummy_ioctl_func
#endif
uint8_t MODULE_IOCTL_FUNCTION(struct dev_desc_t *adev,
		 uint8_t aIoctl_num, void * aIoctl_param1, void * aIoctl_param2);

#ifndef	MODULE_PWRITE_FUNCTION
	#define MODULE_PWRITE_FUNCTION			DEV_API_dummy_pwrite_func
#endif
size_t MODULE_PWRITE_FUNCTION(struct dev_desc_t *adev,
						const uint8_t *apData, size_t aLength, size_t aOffset);

#ifndef	MODULE_PREAD_FUNCTION
	#define MODULE_PREAD_FUNCTION			DEV_API_dummy_pread_func
#endif
size_t MODULE_PREAD_FUNCTION(struct dev_desc_t *adev,
						uint8_t *apData, size_t aLength, size_t aOffset);

#ifndef	MODULE_CALLBACK_FUNCTION
	#define MODULE_CALLBACK_FUNCTION		DEV_API_dummy_callback_func
#endif
uint8_t MODULE_CALLBACK_FUNCTION(struct dev_desc_t *adev,
		uint8_t aCallback_num, void * aCallback_param1,
		void * aCallback_param2);



#ifndef	MODULE_CONFIG_DATA_STRUCT_TYPE
	#define MODULE_CONFIG_DATA_STRUCT_TYPE    uint8_t
#endif

#ifndef	MODULE_RUNTIME_DATA_STRUCT_TYPE
	#define MODULE_RUNTIME_DATA_STRUCT_TYPE   uint8_t
#endif


#define MODULE_PARAMS_ARR2(module)  params_array_##module
#define MODULE_PARAMS_ARR(module)   MODULE_PARAMS_ARR2(module)

#ifdef CONFIG_USE_RUNTIME_DEVICE_CONFIGURATION_BY_PARAMETER_NAMES
	#ifdef MODULE_CONFIGURABLE_PARAMS_ARRAY
		struct dev_param_t const MODULE_PARAMS_ARR(MODULE_NAME)[] =
								MODULE_CONFIGURABLE_PARAMS_ARRAY;
		#define	POINTER_TO_CONFIGURABLE_PARAMS_ARRAY \
										MODULE_PARAMS_ARR(MODULE_NAME)
		#define	SIZE_OF_POINTER_TO_CONFIGURABLE_PARAMS_ARRAY  \
		  sizeof(MODULE_PARAMS_ARR(MODULE_NAME))/sizeof(struct dev_param_t)
	#else
		#define	POINTER_TO_CONFIGURABLE_PARAMS_ARRAY           NULL
		#define SIZE_OF_POINTER_TO_CONFIGURABLE_PARAMS_ARRAY   0
	#endif
#endif


MODULES_PLACEMENT struct included_module_t MODULE_INST(MODULE_NAME) =
	{
		STRINGIFY(MODULE_NAME),
		MODULE_IOCTL_FUNCTION,
		MODULE_PWRITE_FUNCTION,
		MODULE_PREAD_FUNCTION,
		MODULE_CALLBACK_FUNCTION,
#ifdef CONFIG_USE_RUNTIME_DEVICE_CONFIGURATION_BY_PARAMETER_NAMES
		POINTER_TO_CONFIGURABLE_PARAMS_ARRAY,
		SIZE_OF_POINTER_TO_CONFIGURABLE_PARAMS_ARRAY ,
#endif
		sizeof(MODULE_CONFIG_DATA_STRUCT_TYPE),
		sizeof(MODULE_RUNTIME_DATA_STRUCT_TYPE),
		(void*)MODULE_MAGIC_NUMBER
	};


#endif/*__add_module_h__*/
