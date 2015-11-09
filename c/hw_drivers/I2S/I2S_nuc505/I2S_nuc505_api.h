
#ifndef _I2S_NUC505_API_H_
#define _I2S_NUC505_API_H_

#include "I2S_nuc505_config.h"
#include "dev_managment_api.h" // for device manager defines and typedefs
#include "src/_I2S_nuc505_prerequirements_check.h" // should be after {I2S_nuc505_config.h,dev_managment_api.h}

/*****************  defines  **************/


/**********  define API  types ************/

typedef enum
{
	I2S_SET_VOLUME_IOCTL = IOCTL_LAST_COMMON_IOCTL+1,
	I2S_ENABLE_OUTPUT_IOCTL
}I2S_API_ioctl_t;


/**********  define API  functions  ************/

uint8_t  I2S_nuc505_api_init_dev_descriptor(pdev_descriptor aDevDescriptor);


#include "src/I2S_nuc505_static_dev_macros.h"


#define I2S_NUC505_API_CREATE_STATIC_DEV(dev,dev_name ,callback_dev ) \
			__I2S_NUC505_API_CREATE_STATIC_DEV(dev,dev_name,callback_dev )


#endif
