
#ifndef _dpwm_mixer_API_H_
#define _dpwm_mixer_API_H_

#include "dsp_management_api.h"

/*****************  defines  **************/


/**********  define API  types ************/


typedef enum
{
	IOCTL_DPWM_MIXER_ENABLE_TEST_CLIPPING = IOCTL_DSP_LAST_COMMON_IOCTL+1,
	IOCTL_DPWM_MIXER_DISABLE_TEST_CLIPPING,
	IOCTL_DPWM_MIXER_GET_MAX_OUTPUT_VALUE
} DPWM_MIXER_API_ioctl_t;


/**********  define API  functions  ************/

#ifdef __cplusplus
extern "C" {
#endif

extern char dpwm_mixer_module_name[] ;
#define DPWM_MIXER_DSPT	dpwm_mixer_module_name

#ifdef __cplusplus
}
#endif

#endif
