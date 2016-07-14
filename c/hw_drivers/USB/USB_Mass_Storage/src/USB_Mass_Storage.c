/*
 *
 * file :   USB.c
 *
 *
 *
 *
 *
 */



/********  includes *********************/
#include "_project_typedefs.h"
#include "_project_defines.h"
#include "_project_func_declarations.h"

#include "dev_management_api.h" // for device manager defines and typedefs

//#include "Storage_API.h"
#include "USB_Mass_Storage_api.h"
#include "USB_Mass_Storage.h"

#include <stdlib.h>
#include <string.h>

/********  defines *********************/


/********  types  *********************/

/********  externals *********************/


/********  local defs *********************/


USB_Mass_Storage_Instance_t USB_Mass_Storage_InstanceParams = {0} ;





/*---------------------------------------------------------------------------------------------------------*/
/* Function:        USB_Mass_Storage_API_Init                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t  USB_Mass_Storage_API_Init(USB_Mass_Storage_API_Init_params_t *apInit_params)
{
	USB_Mass_Storage_InstanceParams.chip = apInit_params->chip;
	USB_Mass_Storage_InstanceParams.BlockCount = apInit_params->BlockCount;
	USB_Mass_Storage_InstanceParams.BlockSize = apInit_params->BlockSize;
	USB_Mass_Storage_InstanceParams.storage_dev = apInit_params->storage_dev;


//	switch(apInit_params->chip)
//	{
//		case USB_Mass_Storage_API_Chip_STM32F10x:
//			USB_Mass_Storage_STM32F10x_Init(&USB_Mass_Storage_InstanceParams);
//			break;
//		default :
//			return 1;
//	}

	return 0;
}
