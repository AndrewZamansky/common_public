/*
 *
 * file :   webrtc_voice_activity_detection.c
 *
 *
 *
 *
 *
 */



/********  includes *********************/

#include "_webrtc_voice_activity_detection_prerequirements_check.h"

#include "webrtc_voice_activity_detection_api.h" //place first to test that header file is self-contained
#include "webrtc_voice_activity_detection.h"
#include "common_dsp_api.h"

#include "auto_init_api.h"

#include "webrtc/common_audio/vad/include/webrtc_vad.h"

/********  defines *********************/
char webrtc_voice_activity_detection_module_name[] = "voice_activity_detection";


/********  types  *********************/

/********  externals *********************/


/********  local defs *********************/

#define INSTANCE(hndl)	((WEBRTC_VOICE_ACTIVITY_DETECTION_Instance_t*)hndl)



/**********   external variables    **************/


/***********   local variables    **************/


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        webrtc_voice_activity_detection_dsp                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
void webrtc_voice_activity_detection_dsp(pdsp_descriptor apdsp , size_t data_len ,
		dsp_pad_t *in_pads[MAX_NUM_OF_OUTPUT_PADS] , dsp_pad_t out_pads[MAX_NUM_OF_OUTPUT_PADS])
{
	float *apCh1In  ;
	int16_t *buff;
	int16_t *tmp_buff;
	uint32_t i;

	WEBRTC_VOICE_ACTIVITY_DETECTION_Instance_t *handle;
	VadInst* vad_handle;

	handle = apdsp->handle;
	vad_handle = handle -> vad_handle;

	apCh1In = in_pads[0]->buff;

	buff= (int16_t*)malloc(data_len * sizeof(int16_t));
	tmp_buff = buff;
	for (i=0 ; i<data_len ; i++ )
	{
		*tmp_buff++ = (int16_t)(((float)0x7fff) * (*apCh1In++));
	}
	handle->voice_was_detected |= (uint8_t) WebRtcVad_Process(vad_handle, 48000 , buff , data_len);
	free(buff);
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        webrtc_voice_activity_detection_ioctl                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t webrtc_voice_activity_detection_ioctl(pdsp_descriptor apdsp ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2)
{
	WEBRTC_VOICE_ACTIVITY_DETECTION_Instance_t *handle;
	VadInst* vad_handle;

	handle = apdsp->handle;
	vad_handle = handle->vad_handle;

	switch(aIoctl_num)
	{
		case IOCTL_DSP_INIT :
			vad_handle = WebRtcVad_Create();
			handle->vad_handle = vad_handle;
			WebRtcVad_Init(vad_handle);
			WebRtcVad_set_mode(vad_handle , 3);
			handle->voice_was_detected = 0;
			break;
		case IOCTL_DSP_DELETE :
			WebRtcVad_Free(vad_handle);
			break;
		case IOCTL_WEBRTC_VOICE_ACTIVITY_DETECTION_GET_RESULT :
			*(float*)aIoctl_param1 = handle->voice_was_detected  ;
			handle->voice_was_detected = 0;
			break;
		case IOCTL_WEBRTC_VOICE_ACTIVITY_DETECTION_SET_AGRESSIVNESS :
			{
				int agressivness;
				agressivness = *(int*)aIoctl_param1;
				handle->agressivness = agressivness;
				if ((0 <= agressivness) && (3 >= agressivness))
				{
					WebRtcVad_set_mode(vad_handle , agressivness);
				}
			}
			break;
	}
	return 0;
}


#if !defined(WEBRTC_POSIX) && !defined(_WIN32)

void once(void (*func)(void)) {
  /* Didn't use InitializeCriticalSection() since there's no race-free context
   * in which to execute it.
   *
   * TODO(kma): Change to different implementation (e.g.
   * InterlockedCompareExchangePointer) to avoid issues similar to
   * http://code.google.com/p/webm/issues/detail?id=467.
   */
//  static CRITICAL_SECTION lock = {(void *)((size_t)-1), -1, 0, 0, 0, 0};
  static int done = 0;

//  EnterCriticalSection(&lock);
  if (!done) {
    func();
    done = 1;
  }
//  LeaveCriticalSection(&lock);
}
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Function:         voice_activity_detection_init                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
void  webrtc_voice_activity_detection_init(void)
{
	DSP_REGISTER_NEW_MODULE(WEBRTC_VOICE_ACTIVITY_DETECTION_API_MODULE_NAME ,webrtc_voice_activity_detection_ioctl ,
			webrtc_voice_activity_detection_dsp , WEBRTC_VOICE_ACTIVITY_DETECTION_Instance_t);
}

AUTO_INIT_FUNCTION(webrtc_voice_activity_detection_init);
