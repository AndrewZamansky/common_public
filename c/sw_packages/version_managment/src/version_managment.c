/*
 *
 * file :   VersionManagment.c
 *
 *
 *
 *
 *
 */



/********  includes *********************/
#include "version_managment_config.h"
#include "dev_managment_api.h" // for device manager defines and typedefs
#include "src/_version_managment_prerequirements_check.h" // should be after {sw_uart_wrapper_config.h,dev_managment_api.h}

#include "version_managment_api.h"

/********  defines *********************/


/********  types  *********************/


/********  externals *********************/


/********  local defs *********************/

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        VersionManagment_read                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
size_t version_managment_pread(const void *aHandle ,uint8_t *apData , size_t aLength, size_t aOffset)
{
	if(aLength > sizeof(VERSION_STR))
	{
		aLength = sizeof(VERSION_STR);
	}
	memcpy(apData,VERSION_STR, aLength);
	return aLength;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ARM_SH_API_Init_Dev_Descriptor                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t  version_managment_api_init_dev_descriptor(pdev_descriptor aDevDescriptor)
{
	if(NULL == aDevDescriptor) return 1;

	aDevDescriptor->handle = NULL;
	aDevDescriptor->pread = version_managment_pread;

	return 0 ;

}
