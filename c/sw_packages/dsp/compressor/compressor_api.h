
#ifndef _compressor_API_H_
#define _compressor_API_H_

#include "compressor_config.h"
#include "dev_managment_api.h" // for device manager defines and typedefs
#include "dsp_managment_api.h" // for device manager defines and typedefs
#include "src/_compressor_prerequirements_check.h" // should be after {compressor_config.h,dev_managment_api.h}

#include "common_dsp_api.h"

/*****************  defines  **************/


/**********  define API  types ************/



typedef enum
{
	IOCTL_COMPRESSOR_SET_TYPE = IOCTL_LAST_COMMON_IOCTL+1,
	IOCTL_COMPRESSOR_SET_HIGH_THRESHOLD,
	IOCTL_COMPRESSOR_SET_LATENCY,
	IOCTL_COMPRESSOR_SET_RATIO
}COMPRESSOR_API_ioctl_t;




/**********  define API  functions  ************/


uint8_t  compressor_api_init_dsp_descriptor(pdsp_descriptor aDspDescriptor);

//#include "src/_compressor_static_dev_macros.h"
//
//#define COMPRESSOR_API_CREATE_STATIC_DEV(dev,dev_name)  __COMPRESSOR_API_CREATE_STATIC_DEV(dev,dev_name )
//

#endif
