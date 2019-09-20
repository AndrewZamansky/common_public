/*
 *
 * file :   mixer2x1.c
 *
 */



/********  includes *********************/
#include "_project_typedefs.h"
#include "_project_defines.h"

#include "dsp_management_api.h"
#include "dsp_management_internal_api.h"


#include "mixer_api.h"
#include "mixer2x1_api.h"
#include "mixer2x1.h"

#include "auto_init_api.h"


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
void mixer2x1_dsp(struct dsp_module_inst_t *adsp)
{
	struct MIXER2X1_Instance_t *handle;
	real_t *apCh1In;
	real_t *apCh2In;
	real_t *apCh1Out  ;
	real_t channels_weights_1;
	real_t channels_weights_2;
	real_t *channels_weights;
	real_t curr_val;
	size_t in_data_len1 ;
	size_t in_data_len2 ;
	size_t out_data_len ;

	handle = (struct MIXER2X1_Instance_t *)adsp->handle;

	dsp_get_input_buffer_from_pad(adsp, 0, &apCh1In, &in_data_len1);
	dsp_get_input_buffer_from_pad(adsp, 1, &apCh2In, &in_data_len2);
	dsp_get_output_buffer_from_pad(adsp, 0, &apCh1Out, &out_data_len);

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
uint8_t mixer2x1_ioctl(struct dsp_module_inst_t *adsp,
		const uint8_t aIoctl_num, void * aIoctl_param1, void * aIoctl_param2)
{
	struct MIXER2X1_Instance_t *handle;
	uint8_t i;
	real_t *channels_weights ;

	handle = (struct MIXER2X1_Instance_t *)adsp->handle;

	channels_weights = handle->channels_weights ;
	switch(aIoctl_num)
	{
		case IOCTL_DSP_INIT :
			channels_weights[0] = 1.0f;
			channels_weights[1] = 1.0f;
			break;

		case IOCTL_MIXER_SET_CHANNEL_WEIGHT :
			i = ((struct set_channel_weight_t *)aIoctl_param1)->channel_num;
			if ( 2 > i)
			{
				channels_weights[i] =
						((struct set_channel_weight_t *)aIoctl_param1)->weight;
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
