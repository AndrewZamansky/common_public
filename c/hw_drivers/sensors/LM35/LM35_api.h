
#ifndef _LM35_API_H_
#define _LM35_API_H_

#include "LM35_config.h"
#include "dev_managment_api.h" // for device manager defines and typedefs
#include "src/_LM35_prerequirements_check.h" // should be after {gpio_stm8_config.h,dev_managment_api.h}

/**********  define API  types ************/


/**********  define API  functions  ************/

#define LM35_API_SERVER_DEVICE_STR  			"server_device"

typedef enum
{
	IOCTL_LM35_SET_SERVER_DEVICE = IOCTL_LAST_COMMON_IOCTL+1,
	IOCTL_LM35_GET_CURRENT_TEMPERATURE_mC,
}LM35_API_ioctl_t;


uint8_t  LM35_api_init_dev_descriptor(pdev_descriptor aDevDescriptor);

#include "src/LM35_static_dev_macros.h"

#define LM35_API_CREATE_STATIC_DEV(dev,dev_name ,adc_server_dev) \
			__LM35_API_CREATE_STATIC_DEV(dev,dev_name ,adc_server_dev)


#endif
