
#ifndef _webrtc_voice_activity_detection_API_H_
#define _webrtc_voice_activity_detection_API_H_


#include "src/_webrtc_voice_activity_detection_prerequirements_check.h" // should be after {webrtc_voice_activity_detection_config.h,dev_management_api.h}

#include "common_dsp_api.h"

/*****************  defines  **************/


/**********  define API  types ************/

typedef enum
{
	IOCTL_WEBRTC_VOICE_ACTIVITY_DETECTION_GET_RESULT = IOCTL_DSP_LAST_COMMON_IOCTL+1,
	IOCTL_WEBRTC_VOICE_ACTIVITY_DETECTION_SET_AGRESSIVNESS
}WEBRTC_VOICE_ACTIVITY_DETECTION_API_ioctl_t;


/**********  define API  functions  ************/

extern char webrtc_voice_activity_detection_module_name[] ;
#define WEBRTC_VOICE_ACTIVITY_DETECTION_API_MODULE_NAME	webrtc_voice_activity_detection_module_name


#endif
