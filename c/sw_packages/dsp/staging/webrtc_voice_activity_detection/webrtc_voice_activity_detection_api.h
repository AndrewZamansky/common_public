
#ifndef _webrtc_voice_activity_detection_API_H_
#define _webrtc_voice_activity_detection_API_H_


#include "dsp_management_api.h"

enum WEBRTC_VOICE_ACTIVITY_DETECTION_API_ioctl_e {
	IOCTL_WEBRTC_VOICE_ACTIVITY_DETECTION_GET_RESULT =
								IOCTL_DSP_LAST_COMMON_IOCTL + 1,
	IOCTL_WEBRTC_VOICE_ACTIVITY_DETECTION_SET_AGRESSIVNESS
};



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
