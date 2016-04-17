/*
 *
 * file :   speex_echo_canceller.c
 *
 *
 *
 *
 *
 */



/********  includes *********************/

#include "speex_echo_canceller_config.h"
#include "dev_managment_api.h" // for device manager defines and typedefs
#include "dsp_managment_api.h" // for device manager defines and typedefs
#include "_speex_echo_canceller_prerequirements_check.h" // should be after {speex_echo_canceller_config.h,dev_managment_api.h}

#include "speex_echo_canceller_api.h" //place first to test that header file is self-contained
#include "speex_echo_canceller.h"
#include "common_dsp_api.h"

#include "math.h"

#include "speex/speex_echo.h"
#include "speex/speex_preprocess.h"

#ifdef PROJECT_USE_DSP
  #include "cpu_config.h"
  #include "arm_math.h"
#endif

/********  defines *********************/


/********  types  *********************/

/********  externals *********************/


/********  local defs *********************/

#define INSTANCE(hndl)	((SPEEX_ECHO_CANCELLER_Instance_t*)hndl)

#define TAIL 32//1024
#define BUFF_LEN 7
#define SAMPLE_RATE 44100

/**********   external variables    **************/



/***********   local variables    **************/
#if SPEEX_ECHO_CANCELLER_CONFIG_NUM_OF_DYNAMIC_INSTANCES>0

	static SPEEX_ECHO_CANCELLER_Instance_t SPEEX_ECHO_CANCELLER_InstanceParams[SPEEX_ECHO_CANCELLER_CONFIG_NUM_OF_DYNAMIC_INSTANCES] = { {0} };
	static uint16_t usedInstances =0 ;


#endif // for SPEEX_ECHO_CANCELLER_CONFIG_NUM_OF_DYNAMIC_INSTANCES>0


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        speex_echo_canceller_dsp                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
void speex_echo_canceller_dsp(const void * const aHandle , size_t data_len ,
		dsp_pad_t *in_pads[MAX_NUM_OF_OUTPUT_PADS] , dsp_pad_t out_pads[MAX_NUM_OF_OUTPUT_PADS])
{

	int16_t *apCh1In ,  *apCh2In ;
	int16_t *apCh1Out  ;

	apCh1In = (int16_t *)in_pads[0]->buff;
	apCh2In = (int16_t *)in_pads[1]->buff;
	apCh1Out = (int16_t *)out_pads[0].buff;

    speex_echo_cancellation(INSTANCE(aHandle)->echo_state , apCh1In, apCh2In, apCh1Out);
    speex_preprocess_run(INSTANCE(aHandle)->preprocess_state , apCh1Out);
}




/*---------------------------------------------------------------------------------------------------------*/
/* Function:        speex_echo_canceller_ioctl                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t speex_echo_canceller_ioctl(void * const aHandle ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2)
{
	SpeexEchoState *echo_state;
	SpeexPreprocessState *preprocess_state;
	int sampleRate = SAMPLE_RATE;

	switch(aIoctl_num)
	{
//#if SPEEX_ECHO_CANCELLER_CONFIG_NUM_OF_DYNAMIC_INSTANCES > 0
//		case IOCTL_GET_PARAMS_ARRAY_FUNC :
//			*(const dev_param_t**)aIoctl_param1  = SPEEX_ECHO_CANCELLER_Dev_Params;
//			*(uint8_t*)aIoctl_param2 = sizeof(SPEEX_ECHO_CANCELLER_Dev_Params)/sizeof(dev_param_t); //size
//			break;
//#endif // for SPEEX_ECHO_CANCELLER_CONFIG_NUM_OF_DYNAMIC_INSTANCES > 0


		case IOCTL_DEVICE_START :
			echo_state =  speex_echo_state_init(BUFF_LEN , TAIL);
			preprocess_state =  speex_preprocess_state_init(BUFF_LEN , SAMPLE_RATE);
			INSTANCE(aHandle)->echo_state = echo_state ;
			INSTANCE(aHandle)->preprocess_state = preprocess_state ;
			speex_echo_ctl(echo_state, SPEEX_ECHO_SET_SAMPLING_RATE, &sampleRate);
			speex_preprocess_ctl(preprocess_state, SPEEX_PREPROCESS_SET_ECHO_STATE, echo_state);
			break;

	}
	return 0;
}





#if SPEEX_ECHO_CANCELLER_CONFIG_NUM_OF_DYNAMIC_INSTANCES>0

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SPEEX_ECHO_CANCELLER_API_Init_Dev_Descriptor                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t  speex_echo_canceller_api_init_dsp_descriptor(pdsp_descriptor aDspDescriptor)
{
	SPEEX_ECHO_CANCELLER_Instance_t *pInstance;
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


