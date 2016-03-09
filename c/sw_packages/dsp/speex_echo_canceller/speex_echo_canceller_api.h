
#ifndef _speex_echo_canceller_API_H_
#define _speex_echo_canceller_API_H_

#include "speex_echo_canceller_config.h"
#include "dev_managment_api.h" // for device manager defines and typedefs
#include "dsp_managment_api.h" // for device manager defines and typedefs
#include "src/_speex_echo_canceller_prerequirements_check.h" // should be after {speex_echo_canceller_config.h,dev_managment_api.h}

#include "common_dsp_api.h"

/*****************  defines  **************/


/**********  define API  types ************/



//typedef enum
//{
//	IOCTL_SPEEX_ECHO_CANCELLER_SET_GAIN = IOCTL_LAST_COMMON_IOCTL+1,
//}SPEEX_ECHO_CANCELLER_API_ioctl_t;



/**********  define API  functions  ************/


uint8_t  speex_echo_canceller_api_init_dsp_descriptor(pdsp_descriptor aDspDescriptor);

//#include "src/_speex_echo_canceller_static_dev_macros.h"
//
//#define SPEEX_ECHO_CANCELLER_API_CREATE_STATIC_DEV(dev,dev_name)  __SPEEX_ECHO_CANCELLER_API_CREATE_STATIC_DEV(dev,dev_name )
//

#endif