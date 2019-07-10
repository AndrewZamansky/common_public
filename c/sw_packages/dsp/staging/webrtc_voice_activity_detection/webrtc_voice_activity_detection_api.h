
#ifndef _webrtc_voice_activity_detection_API_H_
#define _webrtc_voice_activity_detection_API_H_


#include "dsp_management_api.h"

/*****************  defines  **************/


/**********  define API  types ************/

typedef enum
{
	IOCTL_WEBRTC_VOICE_ACTIVITY_DETECTION_GET_RESULT =
								IOCTL_DSP_LAST_COMMON_IOCTL + 1,
	IOCTL_WEBRTC_VOICE_ACTIVITY_DETECTION_SET_AGRESSIVNESS
}WEBRTC_VOICE_ACTIVITY_DETECTION_API_ioctl_t;


/**********  define API  functions  ************/

#ifdef __cplusplus
extern "C" {
#endif

extern char webrtc_voice_activity_detection_module_name[] ;

#define WEBRTC_VOICE_ACTIVITY_DETECTION_DSPT	\
								webrtc_voice_activity_detection_module_name


#ifdef __cplusplus
}
#endif

#endif
