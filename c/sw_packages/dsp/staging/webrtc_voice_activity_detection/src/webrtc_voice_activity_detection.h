/*
 * file : WEBRTC_VOICE_ACTIVITY_DETECTION.h
 *
 *
 *
 */

#ifndef _WEBRTC_VOICE_ACTIVITY_DETECTION_H
#define _WEBRTC_VOICE_ACTIVITY_DETECTION_H


#include "webrtc/common_audio/vad/include/webrtc_vad.h"

/***************   typedefs    *******************/


struct WEBRTC_VOICE_ACTIVITY_DETECTION_Instance_t {
	VadInst* vad_handle;
	uint8_t voice_was_detected;
	int	agressivness;
};




#endif /* */
