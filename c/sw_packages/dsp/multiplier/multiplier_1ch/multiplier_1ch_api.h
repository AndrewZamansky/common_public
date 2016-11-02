
#ifndef _multiplier_1ch_API_H_
#define _multiplier_1ch_API_H_

#include "src/_multiplier_1ch_prerequirements_check.h"

#include "common_dsp_api.h"

/*****************  defines  **************/


/**********  define API  types ************/


typedef enum
{
	IOCTL_MULTIPLIER_SET_WEIGHT = IOCTL_DSP_LAST_COMMON_IOCTL+1,
}MULTIPLIER_1CH_API_ioctl_t;


/**********  define API  functions  ************/

extern char multiplier_1ch_module_name[] ;
#define MULTIPLIER_1CH_API_MODULE_NAME	multiplier_1ch_module_name



#endif
