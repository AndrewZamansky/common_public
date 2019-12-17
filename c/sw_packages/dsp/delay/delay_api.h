
#ifndef _delay_API_H_
#define _delay_API_H_

#include "dsp_management_api.h"

enum DELAY_API_ioctl_e {
	IOCTL_DELAY_SET_DELAY_SAMPLES = IOCTL_DSP_LAST_COMMON_IOCTL + 1,
};



#ifdef __cplusplus
extern "C" {
#endif

extern char delay_module_name[] ;
#define DELAY_DSPT	delay_module_name

#ifdef __cplusplus
}
#endif

#endif
