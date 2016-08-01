
#ifndef _voice_3D_API_H_
#define _voice_3D_API_H_

#include "src/_voice_3D_prerequirements_check.h"

#include "common_dsp_api.h"

/*****************  defines  **************/


/**********  define API  types ************/



typedef enum
{
	IOCTL_VOICE_3D_SET_MEDIUM_GAIN = IOCTL_LAST_COMMON_IOCTL+1,
	IOCTL_VOICE_3D_SET_SIDE_GAIN,
	IOCTL_VOICE_3D_SET_3D_GAIN,
}VOICE_3D_API_ioctl_t;



/**********  define API  functions  ************/


uint8_t  voice_3D_api_init_dsp_descriptor(pdsp_descriptor aDspDescriptor);

#endif
