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
#include "src/_version_management_prerequirements_check.h"

#include "version_management_api.h"

/********  defines *********************/


/********  types  *********************/


/********  externals *********************/

/********  globals *********************/

/***********************************/
/********** ver_dev ********/
#define DT_DEV_NAME							ver_dev
#define DT_DEV_DRIVER						version_management

#include ADD_CURRENT_DEV

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
size_t version_management_pread(const void *aHandle ,uint8_t *apData , size_t aLength, size_t aOffset)
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
uint8_t  version_management_api_init_dev_descriptor(pdev_descriptor_t aDevDescriptor)
{
	if(NULL == aDevDescriptor) return 1;

	aDevDescriptor->handle = NULL;
	aDevDescriptor->pread = version_management_pread;

	return 0 ;

}
