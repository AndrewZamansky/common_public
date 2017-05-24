
#ifndef _standard_compressor_API_H_
#define _standard_compressor_API_H_

#include "dsp_management_api.h"


/*****************  defines  **************/


/**********  define API  types ************/



typedef enum
{
	IOCTL_STANDARD_COMPRESSOR_SET_GAIN_DB = IOCTL_DSP_LAST_COMMON_IOCTL+1,
	IOCTL_STANDARD_COMPRESSOR_SET_HIGH_THRESHOLD_DB,
	IOCTL_STANDARD_COMPRESSOR_SET_ATTACK_mS,
	IOCTL_STANDARD_COMPRESSOR_SET_RELEASE_mS,
	IOCTL_STANDARD_COMPRESSOR_SET_RATIO,
	IOCTL_STANDARD_COMPRESSOR_SET_ALPHA,
	IOCTL_STANDARD_COMPRESSOR_GET_PARAMS,
}STANDARD_COMPRESSOR_API_ioctl_t;


struct standard_compressor_api_set_params_t
{
	float gain;
	float threshold;
	float attack;
	float release;
	float ratio;
	float alpha;
};

/**********  define API  functions  ************/

extern char standard_compressor_module_name[] ;
#define STANDARD_COMPRESSOR_API_MODULE_NAME		standard_compressor_module_name

#endif
