
#ifndef _multiplier_API_H_
#define _multiplier_API_H_

#include "dsp_management_api.h"


enum MULTIPLIER_API_ioctl_e {
	IOCTL_MULTIPLIER_SET_WEIGHT = IOCTL_DSP_LAST_COMMON_IOCTL + 1,
	IOCTL_MULTIPLIER_SET_WEIGHT_DB,
	IOCTL_MULTIPLIER_GET_PARAMS
};

struct multiplier_api_set_params_t
{
	float weight_db;
};


#endif
