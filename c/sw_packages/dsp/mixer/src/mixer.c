/*
 *
 * file :   mixer.c
 *
 *
 *
 *
 *
 */



/********  includes *********************/

#include "_mixer_prerequirements_check.h"

#include "mixer_api.h" //place first to test that header file is self-contained
#include "mixer.h"
#include "common_dsp_api.h"


/********  defines *********************/


/********  types  *********************/

/********  externals *********************/


/********  local defs *********************/



/**********   external variables    **************/



/***********   local variables    **************/

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        mixer_dsp                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
void mixer_dsp(pdsp_descriptor apdsp , size_t data_len ,
		dsp_pad_t *in_pads[MAX_NUM_OF_OUTPUT_PADS] , dsp_pad_t out_pads[MAX_NUM_OF_OUTPUT_PADS])
{
	MIXER_Instance_t *handle;
	uint8_t	num_of_input_channels;
	float *apCh1In ,  *apCh2In;
	float *apCh1Out  ;
	float channels_weights_0,channels_weights_1  ;

	handle = apdsp->handle;
	apCh1In = in_pads[0]->buff;
	apCh2In = in_pads[1]->buff;
	apCh1Out = out_pads[0].buff;

	float curr_val;
	float *channels_weights;
	channels_weights = handle->channels_weights;
	num_of_input_channels = handle->num_of_input_channels;

	channels_weights_0 =  channels_weights[0];
	channels_weights_1 =  channels_weights[1];

	switch(num_of_input_channels)
	{
		case 2:
			for( ; data_len ;data_len--)
			{
				curr_val = (*apCh1In++) * channels_weights_0;
				curr_val += (*apCh2In++) * channels_weights_1;
				*apCh1Out++ = curr_val;
			}
			break;

		default:
			break;
	}
}





/*---------------------------------------------------------------------------------------------------------*/
/* Function:        mixer_ioctl                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t mixer_ioctl(pdsp_descriptor apdsp ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2)
{
	MIXER_Instance_t *handle;
	uint8_t i;
	uint8_t num_of_input_channels;
	float *channels_weights ;

	handle = apdsp->handle;
	channels_weights = handle->channels_weights ;
	switch(aIoctl_num)
	{
//#if MIXER_CONFIG_NUM_OF_DYNAMIC_INSTANCES > 0
//		case IOCTL_GET_PARAMS_ARRAY_FUNC :
//			*(const dev_param_t**)aIoctl_param1  = MIXER_Dev_Params;
//			*(uint8_t*)aIoctl_param2 = sizeof(MIXER_Dev_Params)/sizeof(dev_param_t); //size
//			break;
//#endif // for MIXER_CONFIG_NUM_OF_DYNAMIC_INSTANCES > 0


		case IOCTL_DEVICE_START :


			break;
		case IOCTL_MIXER_SET_NUM_OF_CHANNELS :
			num_of_input_channels = (uint8_t)((size_t)aIoctl_param1);
			handle->num_of_input_channels = num_of_input_channels;
			channels_weights=(float *)realloc(channels_weights , sizeof(float) * num_of_input_channels);
			handle->channels_weights = channels_weights;
			for(i=0 ; i<num_of_input_channels ; i++)
			{
				channels_weights[i] = 0;
			}

			break;
		case IOCTL_MIXER_SET_CHANNEL_WEIGHT :
			i = ((set_channel_weight_t*)aIoctl_param1)->channel_num;
			channels_weights[i] = ((set_channel_weight_t*)aIoctl_param1)->weight;
			break;
		default :
			return 1;
	}
	return 0;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        MIXER_API_Init_Dev_Descriptor                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t  mixer_api_init_dsp_descriptor(pdsp_descriptor aDspDescriptor)
{
	MIXER_Instance_t *pInstance;

	if(NULL == aDspDescriptor) return 1;

	pInstance = (MIXER_Instance_t *)malloc(sizeof(MIXER_Instance_t));
	if(NULL == pInstance) return 1;

	aDspDescriptor->handle = pInstance;
	aDspDescriptor->ioctl = mixer_ioctl;
	aDspDescriptor->dsp_func = mixer_dsp;

	pInstance->num_of_input_channels =0;
	pInstance->channels_weights=NULL;

	return 0 ;

}
