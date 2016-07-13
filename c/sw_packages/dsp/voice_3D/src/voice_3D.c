/*
 *
 * file :   voice_3D.c
 *
 *
 *
 *
 *
 */



/********  includes *********************/

#include "_voice_3D_prerequirements_check.h" // should be after {voice_3D_config.h,dev_management_api.h}

#include "voice_3D_api.h" //place first to test that header file is self-contained
#include "voice_3D.h"
#include "common_dsp_api.h"

#include "math.h"

#ifdef PROJECT_USE_DSP
  #include "cpu_config.h"
  #include "arm_math.h"
#endif

/********  defines *********************/


/********  types  *********************/

/********  externals *********************/


/********  local defs *********************/

#define INSTANCE(hndl)	((VOICE_3D_Instance_t*)hndl)


/**********   external variables    **************/



/***********   local variables    **************/


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        voice_3D_dsp                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
void voice_3D_dsp(const void * const aHandle , size_t data_len ,
		dsp_pad_t *in_pads[MAX_NUM_OF_OUTPUT_PADS] , dsp_pad_t out_pads[MAX_NUM_OF_OUTPUT_PADS])
{

	float *apCh1In ,  *apCh2In;
	float *apCh1Out ,  *apCh2Out;

	float main_ch_gain;
	float second_ch_gain;

	float medium_gain;
	float side_gain;
	float _3D_gain;

	apCh1In = in_pads[0]->buff;
	apCh2In = in_pads[1]->buff;
	apCh1Out = out_pads[0].buff;
	apCh2Out = out_pads[1].buff;

	medium_gain = INSTANCE(aHandle)->medium_gain ;
	side_gain = INSTANCE(aHandle)->side_gain ;
	_3D_gain = INSTANCE(aHandle)->_3D_gain ;

	main_ch_gain = medium_gain + side_gain ;
	second_ch_gain = medium_gain + side_gain + _3D_gain ;

	while(data_len--)
	{
		float curr_ch_1;
		float curr_ch_2;
		float tmp;
		float tmp1;

		curr_ch_1 = *apCh1In++;
		curr_ch_2 = *apCh2In++;

		tmp = main_ch_gain * curr_ch_1;
		tmp1 = curr_ch_2 * second_ch_gain;
		tmp +=tmp1;
		*apCh1Out++ = tmp;

		tmp = main_ch_gain * curr_ch_2;
		tmp1 = curr_ch_1 * second_ch_gain;
		tmp +=tmp1;
		*apCh2Out++ = tmp;
	}

}




/*---------------------------------------------------------------------------------------------------------*/
/* Function:        voice_3D_ioctl                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t voice_3D_ioctl(void * const aHandle ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2)
{

	switch(aIoctl_num)
	{
//#if VOICE_3D_CONFIG_NUM_OF_DYNAMIC_INSTANCES > 0
//		case IOCTL_GET_PARAMS_ARRAY_FUNC :
//			*(const dev_param_t**)aIoctl_param1  = VOICE_3D_Dev_Params;
//			*(uint8_t*)aIoctl_param2 = sizeof(VOICE_3D_Dev_Params)/sizeof(dev_param_t); //size
//			break;
//#endif // for VOICE_3D_CONFIG_NUM_OF_DYNAMIC_INSTANCES > 0


		case IOCTL_DEVICE_START :

			break;
		case IOCTL_VOICE_3D_SET_MEDIUM_GAIN :
			INSTANCE(aHandle)->medium_gain = (*((float*)aIoctl_param1))/2;
			break;
		case IOCTL_VOICE_3D_SET_SIDE_GAIN :
			INSTANCE(aHandle)->side_gain = (*((float*)aIoctl_param1))/2;
			break;
		case IOCTL_VOICE_3D_SET_3D_GAIN :
			INSTANCE(aHandle)->_3D_gain = *((float*)aIoctl_param1);
			break;
		default :
			return 1;
	}
	return 0;
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        VOICE_3D_API_Init_Dev_Descriptor                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t  voice_3D_api_init_dsp_descriptor(pdsp_descriptor aDspDescriptor)
{
	VOICE_3D_Instance_t *pInstance;

	if(NULL == aDspDescriptor) return 1;

	pInstance = (VOICE_3D_Instance_t *)malloc(sizeof(VOICE_3D_Instance_t));
	if(NULL == pInstance) return 1;

	aDspDescriptor->handle = pInstance;
	aDspDescriptor->ioctl = voice_3D_ioctl;
	aDspDescriptor->dsp_func = voice_3D_dsp;
	pInstance->medium_gain = 0.5;
	pInstance->side_gain =  0.5;
	pInstance->_3D_gain = 0;

	return 0 ;

}
