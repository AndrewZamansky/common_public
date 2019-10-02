/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include "flash_wrapper_api.h"
#include "src/flash_wrapper.h"

#define MODULE_CONFIG_DATA_STRUCT_TYPE   struct flash_wrapper_cfg_t
#define MODULE_RUNTIME_DATA_STRUCT_TYPE  struct flash_wrapper_runtime_t


#ifndef FLASH_WRAPPER_DT_HW_FLASH_PDEV
	#error "FLASH_WRAPPER_DT_HW_FLASH_PDEV should be defined"
#endif

EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(FLASH_WRAPPER_DT_HW_FLASH_PDEV) ;

#define STATIC_DEV_DATA_STRUCT        \
	{                                 \
		P_TO_STATIC_DEVICE_INST(FLASH_WRAPPER_DT_HW_FLASH_PDEV)  \
	}

#include "add_static_dev.h"
