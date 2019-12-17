/*
 *
 * file :   webrtc_echo_canceller.c
 *
 *
 */

#include "_project_typedefs.h"
#include "_project_defines.h"

#include "dsp_management_api.h"


#include "webrtc_echo_canceller_api.h"
#include "webrtc_echo_canceller.h"
#include "common_dsp_api.h"


#include "webrtc/base/checks.h"
#include "webrtc/base/platform_file.h"
#include "webrtc/base/trace_event.h"
#include "webrtc/common_audio/audio_converter.h"
#include "webrtc/common_audio/channel_buffer.h"
#include "webrtc/common_audio/include/audio_util.h"
#include "webrtc/common_audio/signal_processing/include/signal_processing_library.h"
#include "webrtc/modules/audio_processing/aec/aec_core.h"
#include "webrtc/modules/audio_processing/agc/agc_manager_direct.h"
#include "webrtc/modules/audio_processing/audio_buffer.h"
#include "webrtc/modules/audio_processing/beamformer/nonlinear_beamformer.h"
#include "webrtc/modules/audio_processing/common.h"
#include "webrtc/modules/audio_processing/echo_cancellation_impl.h"
#include "webrtc/modules/audio_processing/echo_control_mobile_impl.h"
#include "webrtc/modules/audio_processing/gain_control_for_experimental_agc.h"
#include "webrtc/modules/audio_processing/gain_control_impl.h"
#include "webrtc/modules/audio_processing/high_pass_filter_impl.h"
#include "webrtc/modules/audio_processing/intelligibility/intelligibility_enhancer.h"
#include "webrtc/modules/audio_processing/level_controller/level_controller.h"
#include "webrtc/modules/audio_processing/level_estimator_impl.h"
#include "webrtc/modules/audio_processing/noise_suppression_impl.h"
#include "webrtc/modules/audio_processing/transient/transient_suppressor.h"
#include "webrtc/modules/audio_processing/voice_detection_impl.h"
#include "webrtc/modules/include/module_common_types.h"
#include "webrtc/system_wrappers/include/file_wrapper.h"
#include "webrtc/system_wrappers/include/logging.h"
#include "webrtc/system_wrappers/include/metrics.h"


#define INSTANCE(hndl)	((WEBRTC_ECHO_CANCELLER_Instance_t*)hndl)

#define TAIL 32//1024
#define BUFF_LEN 7
#define SAMPLE_RATE 44100


#define WEBRTC_ECHO_CANCELLER_CONFIG_NUM_OF_DYNAMIC_INSTANCES 1

#if WEBRTC_ECHO_CANCELLER_CONFIG_NUM_OF_DYNAMIC_INSTANCES>0

	static WEBRTC_ECHO_CANCELLER_Instance_t WEBRTC_ECHO_CANCELLER_InstanceParams[WEBRTC_ECHO_CANCELLER_CONFIG_NUM_OF_DYNAMIC_INSTANCES] = { {0} };
	static uint16_t usedInstances =0 ;


#endif // for WEBRTC_ECHO_CANCELLER_CONFIG_NUM_OF_DYNAMIC_INSTANCES>0


/*
 * Function:        webrtc_echo_canceller_dsp
 */
void webrtc_echo_canceller_dsp(pdsp_descriptor apdsp , size_t data_len ,
		dsp_pad_t *in_pads[MAX_NUM_OF_OUTPUT_PADS] , dsp_pad_t out_pads[MAX_NUM_OF_OUTPUT_PADS])
{

//	WEBRTC_ECHO_CANCELLER_Instance_t *handle;
//	int16_t *apCh1In ,  *apCh2In ;
//	int16_t *apCh1Out  ;
//
//	apCh1In = (int16_t *)in_pads[0]->buff;
//	apCh2In = (int16_t *)in_pads[1]->buff;
//	apCh1Out = (int16_t *)out_pads[0].buff;
//
//	handle = apdsp->handle;
//    webrtc_echo_cancellation(handle->echo_state , apCh1In, apCh2In, apCh1Out);
//    webrtc_preprocess_run(handle->preprocess_state , apCh1Out);
}


namespace webrtc
{
/*
 * Function:        webrtc_echo_canceller_ioctl
 */
uint8_t webrtc_echo_canceller_ioctl(pdsp_descriptor apdsp ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2)
{
	webrtc::EchoCancellationImpl * echoCancellation;

	switch(aIoctl_num)
	{
		case IOCTL_DEVICE_START :
			echoCancellation = new EchoCancellationImpl(NULL,NULL);
			break;

	}
	return 0;
}



}

#if WEBRTC_ECHO_CANCELLER_CONFIG_NUM_OF_DYNAMIC_INSTANCES>0

/*
 * Function:        webrtc_echo_canceller_api_init_dsp_descriptor
 */
extern "C" uint8_t  webrtc_echo_canceller_api_init_dsp_descriptor(pdsp_descriptor aDspDescriptor)
{
	WEBRTC_ECHO_CANCELLER_Instance_t *pInstance;
	if(NULL == aDspDescriptor) return 1;
	if (usedInstances >= WEBRTC_ECHO_CANCELLER_CONFIG_NUM_OF_DYNAMIC_INSTANCES) return 1;

	pInstance = &WEBRTC_ECHO_CANCELLER_InstanceParams[usedInstances ];

	aDspDescriptor->handle = pInstance;
	aDspDescriptor->ioctl = webrtc::webrtc_echo_canceller_ioctl;
	aDspDescriptor->dsp_func = webrtc_echo_canceller_dsp;
	usedInstances++;

	return 0 ;

}
#endif  // for WEBRTC_ECHO_CANCELLER_CONFIG_NUM_OF_DYNAMIC_INSTANCES>0


