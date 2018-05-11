/*
 *
 * file :   FatFs.c
 *
 *
 */



/********  includes *********************/
#include "_project_typedefs.h"
#include "_project_defines.h"

#include "dev_management_api.h"
#include "management_api.h"

#include "FatFs_api.h"

#include "FatFs.h"



/*following line add module to available module list for dynamic device tree*/
#include "FatFs_add_component.h"


/********  defines *********************/



/********  types  *********************/



/********  externals *********************/

/* ----------- Exported variables ------------------------*/

/********  local defs *********************/

struct dev_desc_t *   storage_dev ;


/**
 * FatFs_ioctl()
 *
 * return:
 */
uint8_t FatFs_ioctl( struct dev_desc_t *adev ,
		const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2)
{
	struct FatFs_instance_t *config_handle;

	config_handle = DEV_GET_CONFIG_DATA_POINTER(adev);

	switch(aIoctl_num)
	{

	case IOCTL_DEVICE_START :
		storage_dev = config_handle->storage_dev;
		DEV_IOCTL_0_PARAMS(storage_dev, IOCTL_DEVICE_START);

		break;

	default :
		return 1;
	}
	return 0;
}
