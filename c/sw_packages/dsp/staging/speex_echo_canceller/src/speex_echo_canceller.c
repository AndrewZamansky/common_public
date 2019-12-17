/*
 *
 * file :   speex_echo_canceller.c
 *
 *
 *
 *
 *
 */
#include "speex_echo_canceller_config.h"

#include "dsp_management_api.h"

#include "speex_echo_canceller_api.h"
#include "speex_echo_canceller.h"
#include "common_dsp_api.h"

#include "math.h"

#include "speex/speex_echo.h"
#include "speex/speex_preprocess.h"


#define INSTANCE(hndl)	((SPEEX_ECHO_CANCELLER_Instance_t*)hndl)

#define TAIL 32//1024
#define BUFF_LEN 7
#define SAMPLE_RATE 44100


#if SPEEX_ECHO_CANCELLER_CONFIG_NUM_OF_DYNAMIC_INSTANCES>0

	static SPEEX_ECHO_CANCELLER_Instance_t
		SPEEX_ECHO_CANCELLER_InstanceParams[SPEEX_ECHO_CANCELLER_CONFIG_NUM_OF_DYNAMIC_INSTANCES] = { {0} };
	static uint16_t usedInstances =0 ;


#endif // for SPEEX_ECHO_CANCELLER_CONFIG_NUM_OF_DYNAMIC_INSTANCES>0


/*
 * Function:        speex_echo_canceller_dsp
 */
void speex_echo_canceller_dsp(struct dsp_module_inst_t *adsp , size_t data_len ,
		struct dsp_pad_t *in_pads[MAX_NUM_OF_OUTPUT_PADS] ,
		struct dsp_pad_t out_pads[MAX_NUM_OF_OUTPUT_PADS])
{

	struct SPEEX_ECHO_CANCELLER_Instance_t *handle;
	int16_t *apCh1In ,  *apCh2In ;
	int16_t *apCh1Out  ;

	apCh1In = (int16_t *)in_pads[0]->buff;
	apCh2In = (int16_t *)in_pads[1]->buff;
	apCh1Out = (int16_t *)out_pads[0].buff;

	handle = adsp->handle;
    speex_echo_cancellation(handle->echo_state , apCh1In, apCh2In, apCh1Out);
    speex_preprocess_run(handle->preprocess_state , apCh1Out);
}




/*
 * Function:        speex_echo_canceller_ioctl
 */
uint8_t speex_echo_canceller_ioctl(
		struct dsp_module_inst_t *adsp ,const uint8_t aIoctl_num ,
		void * aIoctl_param1 , void * aIoctl_param2)
{
	struct SPEEX_ECHO_CANCELLER_Instance_t *handle;
	SpeexEchoState *echo_state;
	SpeexPreprocessState *preprocess_state;
	int sampleRate = SAMPLE_RATE;

	handle = adsp->handle;
	switch(aIoctl_num)
	{


		case IOCTL_DSP_INIT :
			echo_state =  speex_echo_state_init(BUFF_LEN , TAIL);
			preprocess_state =  speex_preprocess_state_init(BUFF_LEN , SAMPLE_RATE);
			handle->echo_state = echo_state ;
			handle->preprocess_state = preprocess_state ;
			speex_echo_ctl(echo_state, SPEEX_ECHO_SET_SAMPLING_RATE, &sampleRate);
			speex_preprocess_ctl(preprocess_state, SPEEX_PREPROCESS_SET_ECHO_STATE, echo_state);
			break;

	}
	return 0;
}





#if SPEEX_ECHO_CANCELLER_CONFIG_NUM_OF_DYNAMIC_INSTANCES>0

/*
 * Function:        speex_echo_canceller_api_init_dsp_descriptor
 */
uint8_t  speex_echo_canceller_api_init_dsp_descriptor(
					struct dsp_module_inst_t aDspDescriptor)
{
	struct SPEEX_ECHO_CANCELLER_Instance_t *pInstance;
	if(NULL == aDspDescriptor) return 1;
	if (usedInstances >= SPEEX_ECHO_CANCELLER_CONFIG_NUM_OF_DYNAMIC_INSTANCES) return 1;

	pInstance = &SPEEX_ECHO_CANCELLER_InstanceParams[usedInstances ];

	aDspDescriptor->handle = pInstance;
	aDspDescriptor->ioctl = speex_echo_canceller_ioctl;
	aDspDescriptor->dsp_func = speex_echo_canceller_dsp;
	usedInstances++;

	return 0 ;

}
#endif  // for SPEEX_ECHO_CANCELLER_CONFIG_NUM_OF_DYNAMIC_INSTANCES>0


