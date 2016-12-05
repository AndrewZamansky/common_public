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
	float *apCh1Out ;
	uint16_t *buff;
	uint16_t *tmp_buff;
	uint32_t i;

	WEBRTC_VOICE_ACTIVITY_DETECTION_Instance_t *handle;
	VadInst* vad_handle;

	handle = apdsp->handle;
	vad_handle = handle -> vad_handle;

	apCh1In = in_pads[0]->buff;
	apCh1Out = out_pads[0].buff;

	buff= (uint16_t*)malloc(data_len * sizeof(uint16_t));
	tmp_buff = buff;
	for (i=0 ; i<data_len ; i++ )
	{
		*tmp_buff++ = (uint16_t)*apCh1In++;
	}
	apCh1Out[0] = (float)WebRtcVad_Process(vad_handle, 48000 , buff , data_len);
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

	switch(aIoctl_num)
	{
		case IOCTL_DSP_INIT :
			vad_handle = WebRtcVad_Create();
			handle->vad_handle = vad_handle;
			WebRtcVad_Init(vad_handle);
			WebRtcVad_set_mode(vad_handle , 1);
			break;

	}
	return 0;
}





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
	DSP_REGISTER_NEW_MODULE(VOICE_ACTIVITY_DETECTION_API_MODULE_NAME ,webrtc_voice_activity_detection_ioctl ,
			webrtc_voice_activity_detection_dsp , WEBRTC_VOICE_ACTIVITY_DETECTION_Instance_t);
}

AUTO_INIT_FUNCTION(webrtc_voice_activity_detection_init);
