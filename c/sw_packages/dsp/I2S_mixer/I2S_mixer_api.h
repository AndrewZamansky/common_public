
#ifndef _I2S_mixer_API_H_
#define _I2S_mixer_API_H_

#include "src/_I2S_mixer_prerequirements_check.h"

#include "common_dsp_api.h"

/*****************  defines  **************/


/**********  define API  types ************/


typedef enum
{
	IOCTL_I2S_MIXER_ENABLE_TEST_CLIPPING = IOCTL_DSP_LAST_COMMON_IOCTL+1,
	IOCTL_I2S_MIXER_DISABLE_TEST_CLIPPING,
	IOCTL_I2S_MIXER_GET_MAX_OUTPUT_VALUE
}I2S_MIXER_API_ioctl_t;


/**********  define API  functions  ************/

extern char I2S_mixer_module_name[] ;
#define I2S_MIXER_API_MODULE_NAME	I2S_mixer_module_name



#endif
