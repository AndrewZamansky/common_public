
#ifndef _tone_generator_API_H_
#define _tone_generator_API_H_

#include "dsp_management_api.h"


enum TONE_GENERATOR_API_ioctl_e {
	IOCTL_TONE_GENERATOR_SET_FREQUENCY = IOCTL_DSP_LAST_COMMON_IOCTL + 1,
	IOCTL_TONE_GENERATOR_SET_SAMPLE_RATE,
	IOCTL_TONE_GENERATOR_SET_VOLUME_LIN
};


#ifdef __cplusplus
extern "C" {
#endif

extern char tone_generator_module_name[] ;
#define TONE_GENERATOR_DSPT	tone_generator_module_name

#ifdef __cplusplus
}
#endif

#endif
