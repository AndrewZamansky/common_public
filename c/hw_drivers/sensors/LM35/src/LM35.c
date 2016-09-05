/*
 *
 *   file  :  LM35.cpp
 *
 */



/***************   includes    *******************/
#include "LM35_config.h"
#include "dev_management_api.h" // for device manager defines and typedefs
#include "src/_LM35_prerequirements_check.h" // should be after {LM35_config.h,dev_management_api.h}

#include <stddef.h>
#include "LM35_api.h"
#include "adc_api.h"

#include "LM35_add_component.h"

/***************   defines    *******************/


/***************   typedefs    *******************/



/**********   external variables    **************/



/***********   local variables    **************/


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        LM35_ioctl                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t LM35_ioctl( pdev_descriptor_t apdev ,const uint8_t aIoctl_num
		, void * aIoctl_param1 , void * aIoctl_param2)
{
	LM35_Instance_t *handle;

	handle = apdev->handle;
	switch(aIoctl_num)
	{
#if LM35_CONFIG_NUM_OF_DYNAMIC_INSTANCES > 0
		case IOCTL_LM35_SET_SERVER_DEVICE :
			handle->adc_server_device = DEV_OPEN((uint8_t*)aIoctl_param1);
			break;
#endif
		case IOCTL_LM35_GET_CURRENT_TEMPERATURE_mC :
			if(NULL != handle->adc_server_device)
			{
				DEV_IOCTL(handle->adc_server_device , IOCTL_ADC_GET_CURRENT_VALUE_mV , aIoctl_param1);
				*(uint32_t*)aIoctl_param1 = (*(uint32_t*)aIoctl_param1)*100;
			}
			break;
		case IOCTL_DEVICE_START :
			break;

		default :
			return 1;
	}
	return 0;
}
