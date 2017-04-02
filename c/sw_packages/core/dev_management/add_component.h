/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#ifndef	MODULE_IOCTL_FUNCTION
	#define MODULE_IOCTL_FUNCTION			DEV_API_dummy_ioctl_func
#endif
extern uint8_t MODULE_IOCTL_FUNCTION(pdev_descriptor_t apdev,
		const uint8_t aIoctl_num, void * aIoctl_param1,
		void * aIoctl_param2);

#ifndef	MODULE_PWRITE_FUNCTION
	#define MODULE_PWRITE_FUNCTION			DEV_API_dummy_pwrite_func
#endif
extern size_t MODULE_PWRITE_FUNCTION(const pdev_descriptor_t apdev,
				const uint8_t *apData, size_t aLength, size_t aOffset);

#ifndef	MODULE_PREAD_FUNCTION
	#define MODULE_PREAD_FUNCTION			DEV_API_dummy_pread_func
#endif
extern size_t MODULE_PREAD_FUNCTION(const pdev_descriptor_t apdev,
						uint8_t *apData, size_t aLength, size_t aOffset);

#ifndef	MODULE_CALLBACK_FUNCTION
	#define MODULE_CALLBACK_FUNCTION		DEV_API_dummy_callback_func
#endif
extern uint8_t MODULE_CALLBACK_FUNCTION(pdev_descriptor_t apdev,
		const uint8_t aCallback_num, void * aCallback_param1,
		void * aCallback_param2);


#ifdef DT_DEV_MODULE
	#include "src/add_static_dev.h"
#else
	#include "src/add_module.h"
#endif

#undef	MODULE_NAME
#undef	MODULE_IOCTL_FUNCTION
#undef	MODULE_PWRITE_FUNCTION
#undef	MODULE_PREAD_FUNCTION
#undef	MODULE_CALLBACK_FUNCTION
#undef	MODULE_CONFIG_DATA_STRUCT_TYPE
#undef	MODULE_RUNTIME_DATA_STRUCT_TYPE
#undef	MODULE_CONFIGURABLE_PARAMS_ARRAY
