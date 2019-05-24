
#ifndef _DPWM_I94XXX_API_H_
#define _DPWM_I94XXX_API_H_

#include "dev_management_api.h"
//#include "src/dpwm.h"

/*****************  defines  **************/


/**********  define API  types ************/

enum DPWM_I94XXX_API_ioctl_e {
	DPWM_I94XXX_ENABLE_OUTPUT_IOCTL = IOCTL_LAST_COMMON_IOCTL + 1,
	DPWM_I94XXX_DEVICE_STOP,
	DPWM_I94XXX_GET_ROOT_CLK_DEV,
	DPWM_I94XXX_GET_SAMPLE_RATE_HZ
};



/**********  define API  functions  ************/

#endif
