/*
 *
 * file :   FatFs.c
 *
 *
 */

#include "_project_typedefs.h"
#include "_project_defines.h"

#include "dev_management_api.h"
#include "management_api.h"

#include "FatFs_api.h"

#include "FatFs.h"


// following variable should be moved to runtime_data or to partition array
struct dev_desc_t *   storage_for_fatFs_dev ;


/**
 * FatFs_ioctl()
 *
 * return:
 */
uint8_t FatFs_ioctl( struct dev_desc_t *adev ,
		const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2)
{
	struct FatFs_instance_t *config_handle;

	config_handle = DEV_GET_CONFIG_DATA_POINTER(FatFs, adev);

	switch(aIoctl_num)
	{

	case IOCTL_DEVICE_START :
		storage_for_fatFs_dev = config_handle->storage_dev;
		DEV_IOCTL_0_PARAMS(storage_for_fatFs_dev, IOCTL_DEVICE_START);

		break;

	default :
		return 1;
	}
	return 0;
}

#define MODULE_NAME            FatFs
#define MODULE_IOCTL_FUNCTION  FatFs_ioctl
#define MODULE_HAS_NO_RUNTIME_DATA
#include "add_module.h"
