
#ifndef _PCM_mixer_API_H_
#define _PCM_mixer_API_H_

#include "dsp_management_api.h"

/*****************  defines  **************/


/**********  define API  types ************/


typedef enum
{
	IOCTL_PCM_MIXER_ENABLE_TEST_CLIPPING = IOCTL_DSP_LAST_COMMON_IOCTL + 1,
	IOCTL_PCM_MIXER_DISABLE_TEST_CLIPPING,
	IOCTL_PCM_MIXER_GET_MAX_OUTPUT_VALUE,
	IOCTL_PCM_MIXER_SET_PARAMS
}PCM_MIXER_API_ioctl_t;


enum pcm_mixer_channel_justification_e {
	PCM_MIXER_CHANNEL_BITS_ARE_LEFT_JUSTIFIED,
	PCM_MIXER_CHANNEL_BITS_ARE_RIGHT_JUSTIFIED,
};

struct pcm_mixer_api_set_params_t {
	uint8_t frame_size_bytes; /*frame contains N subframes*/
	uint8_t subframe_size_bytes; /*subframes contain M bits of channel*/
	uint8_t channel_size_bits; /**/
	uint8_t channel_justification; /**/
};

/**********  define API  functions  ************/

extern char pcm_mixer_module_name[] ;
#define PCM_MIXER_DSPT	pcm_mixer_module_name



#endif
