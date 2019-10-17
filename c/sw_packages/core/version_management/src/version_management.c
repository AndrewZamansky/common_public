/*
 *
 * file :   version_management.c
 *
 *
 */



/********  includes *********************/
#include "_project_typedefs.h"
#include "_project_defines.h"

#include "string.h"
#include "dev_management_api.h"

#include "version_management_api.h"


/********  defines *********************/


/********  types  *********************/


/********  externals *********************/

/********  globals *********************/

/***********************************/
/********** ver_dev ********/
#define DT_DEV_NAME    ver_dev
#define DT_DEV_MODULE  version_management

#include ADD_CURRENT_DEV

/********  local defs *********************/


/**
 * version_management_pread()
 *
 * return:
 */
static size_t version_management_pread(struct dev_desc_t *adev,
				uint8_t *apData , size_t aLength, size_t aOffset)
{
	if((aLength + aOffset) > (sizeof(VERSION_STR) - 1))
	{
		if (aOffset >= (sizeof(VERSION_STR) - 1))
		{
			aLength = 0;
		}
		else
		{
			aLength = (sizeof(VERSION_STR) - 1) - aOffset;
		}
	}
	memcpy(apData, &VERSION_STR[aOffset], aLength);
	return aLength;
}

#define	MODULE_NAME             version_management
#define	MODULE_PREAD_FUNCTION   version_management_pread
#define MODULE_HAS_NO_CONFIG_DATA
#define MODULE_HAS_NO_RUNTIME_DATA
#include "add_module.h"
