/*
 *
 *   file  :  dev_placement.c
 *
 */



/***************   includes    *******************/

#include "dev_management_api.h"
#include "_dev_management_prerequirements_check.h"
#include "dev_management.h"

uint32_t START_OF_DEVICE_TREE_PLACEMENT		start_of_device_tree_stamp ;
uint8_t END_OF_DEVICE_TREE_PLACEMENT		end_of_device_tree_stamp = END_OF_DEVICE_TREE_STAMP ;

#if (defined(CONFIG_DYNAMIC_DEVICE_TREE) ||  \
		defined(CONFIG_USE_RUNTIME_DEVICE_CONFIGURATION_BY_PARAMETER_NAMES) || (CONFIG_MAX_NUM_OF_DYNAMIC_DEVICES>0))

	uint32_t START_OF_MODULES_PLACEMENT			start_of_modules_stamp ;
	uint8_t END_OF_MODULES_PLACEMENT			end_of_modules_stamp ;

#endif
