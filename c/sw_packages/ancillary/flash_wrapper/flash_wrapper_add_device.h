/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include "flash_wrapper_api.h"
#include "src/flash_wrapper.h"

#ifndef FLASH_WRAPPER_DT_HW_FLASH_PDEV
	#error "FLASH_WRAPPER_DT_HW_FLASH_PDEV should be defined"
#endif

EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(FLASH_WRAPPER_DT_HW_FLASH_PDEV) ;

SET_STATIC_DEV_CONFIG(flash_wrapper) =
{
	P_TO_STATIC_DEVICE_INST(FLASH_WRAPPER_DT_HW_FLASH_PDEV)
};

#include "add_static_dev.h"
