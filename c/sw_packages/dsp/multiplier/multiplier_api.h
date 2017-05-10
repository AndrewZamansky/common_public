
#ifndef _multiplier_API_H_
#define _multiplier_API_H_

#include "src/_multiplier_2ch_prerequirements_check.h"

#include "common_dsp_api.h"

/*****************  defines  **************/


/**********  define API  types ************/


typedef enum
{
	IOCTL_MULTIPLIER_SET_WEIGHT = IOCTL_DSP_LAST_COMMON_IOCTL + 1,
	IOCTL_MULTIPLIER_GET_PARAMS
}MULTIPLIER_API_ioctl_t;

struct multiplier_api_set_params_t
{
	float weight;
};


/**********  define API  functions  ************/


#endif
