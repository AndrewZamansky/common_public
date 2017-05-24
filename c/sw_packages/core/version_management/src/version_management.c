/*
 *
 * file :   version_management.c
 *
 *
 *
 *
 *
 */



/********  includes *********************/
#include "_project_typedefs.h"
#include "_project_defines.h"

#include "dev_management_api.h"

#include "version_management_api.h"

#include "_version_management_prerequirements_check.h"

#include "version_management_add_component.h"

/********  defines *********************/


/********  types  *********************/


/********  externals *********************/

/********  globals *********************/

/***********************************/
/********** ver_dev ********/
#define DT_DEV_NAME							ver_dev
#define DT_DEV_MODULE						version_management

#include ADD_CURRENT_DEV

/********  local defs *********************/


/**
 * version_management_pread()
 *
 * return:
 */
size_t version_management_pread(struct dev_desc_t *adev,
				uint8_t *apData , size_t aLength, size_t aOffset)
{
	if(aLength > sizeof(VERSION_STR))
	{
		aLength = sizeof(VERSION_STR);
	}
	memcpy(apData,VERSION_STR, aLength);
	return aLength;
}
