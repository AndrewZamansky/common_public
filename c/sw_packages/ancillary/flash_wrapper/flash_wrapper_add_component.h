/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include "flash_wrapper_api.h"
#include "src/flash_wrapper.h"


#define	MODULE_NAME							flash_wrapper
#define	MODULE_IOCTL_FUNCTION				flash_wrapper_ioctl
#define MODULE_PWRITE_FUNCTION				flash_wrapper_pwrite
#define MODULE_PREAD_FUNCTION				flash_wrapper_pread
#define MODULE_CONFIG_DATA_STRUCT_TYPE  	struct flash_wrapper_cfg_t
#define MODULE_RUNTIME_DATA_STRUCT_TYPE 	struct flash_wrapper_runtime_t


#ifdef DT_DEV_MODULE

	#ifndef FLASH_WRAPPER_DT_HW_FLASH_PDEV
		#error "FLASH_WRAPPER_DT_HW_FLASH_PDEV should be defined"
	#endif

	EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(FLASH_WRAPPER_DT_HW_FLASH_PDEV) ;

	#define STATIC_DEV_DATA_STRUCT									 \
		{															 \
			P_TO_STATIC_DEVICE_INST(FLASH_WRAPPER_DT_HW_FLASH_PDEV)  \
		}

#endif

#include "add_component.h"

