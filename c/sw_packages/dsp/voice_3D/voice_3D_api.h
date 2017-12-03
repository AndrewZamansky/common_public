
#ifndef _voice_3D_API_H_
#define _voice_3D_API_H_

#include "dsp_management_api.h"

/*****************  defines  **************/


/**********  define API  types ************/



typedef enum
{
	IOCTL_VOICE_3D_SET_CENTER_GAIN = IOCTL_DSP_LAST_COMMON_IOCTL + 1,
	IOCTL_VOICE_3D_SET_SIDE_GAIN,
	IOCTL_VOICE_3D_SET_3D_GAIN,
	IOCTL_VOICE_3D_SET_VOICE_ON,
	IOCTL_VOICE_3D_SET_3D_ON,
	IOCTL_VOICE_3D_GET_CENTER_GAIN,
	IOCTL_VOICE_3D_GET_SIDE_GAIN,
	IOCTL_VOICE_3D_GET_3D_GAIN,
	IOCTL_VOICE_3D_GET_VOICE_ON,
	IOCTL_VOICE_3D_GET_3D_ON,
}VOICE_3D_API_ioctl_t;



/**********  define API  functions  ************/

extern char voice_3D_module_name[] ;
#define VOICE_3D_API_MODULE_NAME	voice_3D_module_name


#endif
