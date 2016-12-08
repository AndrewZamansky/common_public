/*
 * file : WEBRTC_VOICE_ACTIVITY_DETECTION.h
 *
 *
 *
 *
 *
 *
 */

#ifndef _WEBRTC_VOICE_ACTIVITY_DETECTION_H
#define _WEBRTC_VOICE_ACTIVITY_DETECTION_H

#include "src/_webrtc_voice_activity_detection_prerequirements_check.h"

#include "webrtc/common_audio/vad/include/webrtc_vad.h"

/***************   typedefs    *******************/


typedef struct
{
	VadInst* vad_handle;
	uint8_t voice_was_detected;
	int	agressivness;
} WEBRTC_VOICE_ACTIVITY_DETECTION_Instance_t;




#endif /* */
