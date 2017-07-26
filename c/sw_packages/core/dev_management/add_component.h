/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#ifndef	MODULE_IOCTL_FUNCTION
	#define MODULE_IOCTL_FUNCTION			DEV_API_dummy_ioctl_func
#endif
extern uint8_t MODULE_IOCTL_FUNCTION(struct dev_desc_t *adev,
		 uint8_t aIoctl_num, void * aIoctl_param1, void * aIoctl_param2);

#ifndef	MODULE_PWRITE_FUNCTION
	#define MODULE_PWRITE_FUNCTION			DEV_API_dummy_pwrite_func
#endif
extern size_t MODULE_PWRITE_FUNCTION(struct dev_desc_t *adev,
						const uint8_t *apData, size_t aLength, size_t aOffset);

#ifndef	MODULE_PREAD_FUNCTION
	#define MODULE_PREAD_FUNCTION			DEV_API_dummy_pread_func
#endif
extern size_t MODULE_PREAD_FUNCTION(struct dev_desc_t *adev,
						uint8_t *apData, size_t aLength, size_t aOffset);

#ifndef	MODULE_CALLBACK_FUNCTION
	#define MODULE_CALLBACK_FUNCTION		DEV_API_dummy_callback_func
#endif
extern uint8_t MODULE_CALLBACK_FUNCTION(struct dev_desc_t *adev,
		uint8_t aCallback_num, void * aCallback_param1,
		void * aCallback_param2);

#ifdef CONFIG_USE_SPECIFIC_MEMORY_LOCATION_FOR_DEVICES
	#if (defined(CONFIG_CORTEX_M3) || defined(CONFIG_CORTEX_M4) || 	\
			defined(CONFIG_CORTEX_A9) )  && defined(CONFIG_GCC)
		#define START_OF_MODULES_PLACEMENT	 		__attribute__((section(".start_of_modules")))
		#define MODULES_PLACEMENT	 				__attribute__((section(".modules")))
		#define END_OF_MODULES_PLACEMENT	 		__attribute__((section(".end_of_modules")))

		#define START_OF_DEVICE_TREE_PLACEMENT	 	__attribute__((section(".start_of_device_tree")))
		#define DEVICE_PLACEMENT	 				__attribute__((section(".static_devs")))
		#define END_OF_DEVICE_TREE_PLACEMENT	 	__attribute__((section(".end_of_device_tree")))

		#define DEVICE_DATA_PLACEMENT	 			__attribute__((section(".static_devs_data")))
		#define AUTO_INIT_FUNCTION_PLACEMENT	 	__attribute__((section(".auto_init_functions")))
	#endif

#else
	#define START_OF_MODULES_PLACEMENT
	#define MODULES_PLACEMENT
	#define END_OF_MODULES_PLACEMENT

	#define START_OF_DEVICE_TREE_PLACEMENT
	#define DEVICE_PLACEMENT
	#define END_OF_DEVICE_TREE_PLACEMENT

	#define DEVICE_DATA_PLACEMENT
#endif


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
