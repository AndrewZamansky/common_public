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
#include "src/_version_managment_prerequirements_check.h"

#include "version_managment_api.h"

/********  defines *********************/


/********  types  *********************/


/********  externals *********************/

/********  globals *********************/
VERSION_MANAGMENT_API_CREATE_STATIC_DEV(ver_dev);

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
uint8_t  version_managment_api_init_dev_descriptor(pdev_descriptor_t aDevDescriptor)
{
	if(NULL == aDevDescriptor) return 1;

	aDevDescriptor->handle = NULL;
	aDevDescriptor->pread = version_managment_pread;

	return 0 ;

}
