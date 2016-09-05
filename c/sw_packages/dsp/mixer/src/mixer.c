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

#include "auto_init_api.h"

/********  defines *********************/


/********  types  *********************/

/********  externals *********************/


/********  exported variables *********************/

char mixer_module_name[] = "mixer";

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
		case IOCTL_DSP_INIT :

			handle->num_of_input_channels =0;
			handle->channels_weights=NULL;

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
/* Function:        mixer_init                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
void  mixer_init(void)
{
	DSP_REGISTER_NEW_MODULE("mixer",mixer_ioctl , mixer_dsp , MIXER_Instance_t);
}

AUTO_INIT_FUNCTION(mixer_init);
