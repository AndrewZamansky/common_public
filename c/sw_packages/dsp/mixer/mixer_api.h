
#ifndef _mixer_API_H_
#define _mixer_API_H_

#include "src/_mixer_prerequirements_check.h"

#include "common_dsp_api.h"

/*****************  defines  **************/


/**********  define API  types ************/



typedef enum
{
	IOCTL_MIXER_SET_CHANNEL_WEIGHT = IOCTL_LAST_COMMON_IOCTL+1,
	IOCTL_MIXER_SET_NUM_OF_CHANNELS,
}MIXER_API_ioctl_t;



/**********  define API  functions  ************/


typedef struct _set_channel_weight_t
{
	uint8_t channel_num;
	float weight;
} set_channel_weight_t;

uint8_t  mixer_api_init_dsp_descriptor(pdsp_descriptor aDspDescriptor);

//#include "src/_mixer_static_dev_macros.h"
//
//#define MIXER_API_CREATE_STATIC_DEV(dev,dev_name)  __MIXER_API_CREATE_STATIC_DEV(dev,dev_name )
//

#endif
