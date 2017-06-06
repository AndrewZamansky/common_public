/*
 *
 * file :   mixer2x1.c
 *
 */



/********  includes *********************/
#include "_project_typedefs.h"
#include "_project_defines.h"

#include "dsp_management_api.h"
#include "common_dsp_api.h"


#include "mixer_api.h"
#include "mixer2x1_api.h"
#include "mixer2x1.h"

#include "auto_init_api.h"

#include "_mixer2x1_prerequirements_check.h"

/********  defines *********************/


/********  types  *********************/

/********  externals *********************/


/********  exported variables *********************/

char mixer2x1_module_name[] = "mixer2x1";

/**********   external variables    **************/



/***********   local variables    **************/


/**
 * mixer2x1_dsp()
 *
 * return:
 */
void mixer2x1_dsp(struct dsp_desc_t *adsp,
		struct dsp_pad_t *in_pads[MAX_NUM_OF_OUTPUT_PADS],
		struct dsp_pad_t out_pads[MAX_NUM_OF_OUTPUT_PADS])
{
	struct MIXER2X1_Instance_t *handle;
	float *apCh1In;
	float *apCh2In;
	float *apCh1Out  ;
	float channels_weights_1;
	float channels_weights_2;
	float *channels_weights;
	float curr_val;
	size_t in_data_len1 ;
	size_t in_data_len2 ;
	size_t out_data_len ;

	handle = adsp->handle;

	DSP_GET_BUFFER(in_pads[0], &apCh1In, &in_data_len1);
	DSP_GET_BUFFER(in_pads[1], &apCh2In, &in_data_len2);
	DSP_GET_BUFFER(&out_pads[0], &apCh1Out, &out_data_len);

	if (in_data_len1 != in_data_len2 )
	{
		CRITICAL_ERROR("bad input buffer size");
	}

	if (in_data_len1 > out_data_len )
	{
		CRITICAL_ERROR("bad buffers sizes");
	}

	channels_weights = handle->channels_weights;

	channels_weights_1 =  channels_weights[0];
	channels_weights_2 =  channels_weights[1];

	while( in_data_len1--)
	{
		curr_val = (*apCh1In++) * channels_weights_1;
		curr_val += (*apCh2In++) * channels_weights_2;
		*apCh1Out++ = curr_val;
	}

}




/**
 * mixer2x1_ioctl()
 *
 * return:
 */
uint8_t mixer2x1_ioctl(struct dsp_desc_t *adsp,
		const uint8_t aIoctl_num, void * aIoctl_param1, void * aIoctl_param2)
{
	struct MIXER2X1_Instance_t *handle;
	uint8_t i;
	float *channels_weights ;

	handle = adsp->handle;
	channels_weights = handle->channels_weights ;
	switch(aIoctl_num)
	{
		case IOCTL_DSP_INIT :

			break;
		case IOCTL_MIXER_SET_CHANNEL_WEIGHT :
			i = ((set_channel_weight_t*)aIoctl_param1)->channel_num;
			if( 2> i)
			{
				channels_weights[i] = ((set_channel_weight_t*)aIoctl_param1)->weight;
			}
			break;
		default :
			return 1;
	}
	return 0;
}



/**
 * mixer2x1_init()
 *
 * return:
 */
void  mixer2x1_init(void)
{
	DSP_REGISTER_NEW_MODULE("mixer2x1",
			mixer2x1_ioctl , mixer2x1_dsp , struct MIXER2X1_Instance_t);
}

AUTO_INIT_FUNCTION(mixer2x1_init);
