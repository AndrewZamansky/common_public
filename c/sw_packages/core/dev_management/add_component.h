/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

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
#undef	MODULE_DATA_STRUCT_TYPE
