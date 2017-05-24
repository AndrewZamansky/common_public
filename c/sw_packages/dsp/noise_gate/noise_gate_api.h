
#ifndef _noise_gate_API_H_
#define _noise_gate_API_H_

#include "dsp_management_api.h"


/*****************  defines  **************/


/**********  define API  types ************/



typedef enum
{
	IOCTL_NOISE_GATE_SET_THRESHOLD_DB = IOCTL_DSP_LAST_COMMON_IOCTL + 1,
	IOCTL_NOISE_GATE_SET_ATTACK_mS,
	IOCTL_NOISE_GATE_SET_RELEASE_mS,
	IOCTL_NOISE_GATE_SET_ALPHA,
}NOISE_GATE_API_ioctl_t;



/**********  define API  functions  ************/

extern char noise_gate_module_name[] ;
#define NOISE_GATE_API_MODULE_NAME		noise_gate_module_name

#endif
