
#ifndef _MIXER_API_H_
#define _MIXER_API_H_

#include "dsp_management_api.h"


enum MIXER_API_ioctl_e {
	IOCTL_MIXER_SET_CHANNEL_WEIGHT = IOCTL_DSP_LAST_COMMON_IOCTL + 1,
};


struct set_channel_weight_t
{
	uint8_t channel_num;
	float weight;
};


#endif
