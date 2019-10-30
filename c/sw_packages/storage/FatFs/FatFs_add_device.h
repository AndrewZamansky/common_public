/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include "FatFs_api.h"
#include "src/FatFs.h"


#ifndef FAT_FS_DT_STORAGE_DEV
	#error "FAT_FS_DT_STORAGE_DEV should be defined"
#endif
EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(FAT_FS_DT_STORAGE_DEV) ;
#define _POINTER_TO_STORAGE_DEV  P_TO_STATIC_DEVICE_INST(FAT_FS_DT_STORAGE_DEV)


SET_STATIC_DEV_CONFIG(FatFs) =
{
	_POINTER_TO_STORAGE_DEV
};

#define MODULE_HAS_NO_RUNTIME_DATA


#include "add_static_dev.h"
/**
 *  device specific defines should be undefined
 *  after calling #include "add_static_dev.h"
 */
#undef FAT_FS_DT_STORAGE_DEV
#undef _POINTER_TO_STORAGE_DEV
