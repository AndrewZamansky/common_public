/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include "FatFs_api.h"
#include "src/FatFs.h"


#define MODULE_NAME                      FatFs
#define MODULE_IOCTL_FUNCTION            FatFs_ioctl
#define MODULE_CALLBACK_FUNCTION         FatFs_callback
#define MODULE_CONFIG_DATA_STRUCT_TYPE   struct FatFs_instance_t

#ifdef DT_DEV_MODULE

	#ifndef FAT_FS_DT_STORAGE_DEV
		#error "FAT_FS_DT_STORAGE_DEV should be defined"
	#endif
	EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(FAT_FS_DT_STORAGE_DEV) ;
	#define _POINTER_TO_STORAGE_DEV   \
		P_TO_STATIC_DEVICE_INST(FAT_FS_DT_STORAGE_DEV)


	#define STATIC_DEV_DATA_STRUCT      \
		{                               \
				_POINTER_TO_STORAGE_DEV,\
		};

#endif


#include "add_component.h"
/**
 *  device specific defines should be undefined
 *  after calling #include "add_static_dev.h"
 */
#undef FAT_FS_DT_STORAGE_DEV
#undef _POINTER_TO_STORAGE_DEV
