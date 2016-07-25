/*
 *
 *   file  :  dev_placement.c
 *
 */



/***************   includes    *******************/

#include "dev_management_api.h"
#include "_dev_management_prerequirements_check.h"
#include "dev_management.h"

uint8_t END_OF_DEVICE_TREE_PLACEMENT		end_of_device_tree_stamp = END_OF_DEVICE_TREE_STAMP;
uint32_t START_OF_MODULES_PLACEMENT			start_of_modules_stamp = 0;
uint8_t END_OF_MODULES_PLACEMENT			end_of_modules_stamp = 0;
