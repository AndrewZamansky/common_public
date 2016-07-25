#ifndef __add_module_h__
#define __add_module_h__

#include "dev_management_api.h"

#ifdef CONFIG_DYNAMIC_DEVICE_TREE

#ifndef MODULE_NAME
	#error "MODULE_NAME should be defined"
#endif

#ifndef	MODULE_INIT_FUNCTION
	#define MODULE_INIT_FUNCTION			DEV_API_dummy_init_func
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

#define MODULE_INST2(module)	module_inst_##module
#define MODULE_INST(module)		MODULE_INST2(module)


MODULE_PLACEMENT included_module_t MODULE_INST(MODULE_NAME) =
	{
		STRINGIFY(MODULE_NAME)	,
		MODULE_INIT_FUNCTION	,
		MODULE_IOCTL_FUNCTION 	,
		MODULE_PWRITE_FUNCTION 	,
		MODULE_PREAD_FUNCTION 	,
		MODULE_CALLBACK_FUNCTION
	};
#endif

#endif
