
#ifndef _voice_3D_API_H_
#define _voice_3D_API_H_

#include "dsp_management_api.h"

/*****************  defines  **************/


/**********  define API  types ************/



typedef enum
{
	IOCTL_VOICE_3D_SET_MEDIUM_GAIN = IOCTL_DSP_LAST_COMMON_IOCTL + 1,
	IOCTL_VOICE_3D_SET_SIDE_GAIN,
	IOCTL_VOICE_3D_SET_3D_GAIN,
}VOICE_3D_API_ioctl_t;



/**********  define API  functions  ************/

extern char voice_3D_module_name[] ;
#define VOICE_3D_API_MODULE_NAME	voice_3D_module_name


#endif
