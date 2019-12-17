/*
 *
 * file :   mixer4x1.c
 *
 *
 */
#include "_project_typedefs.h"
#include "_project_defines.h"

#include "dsp_management_api.h"
#include "dsp_management_internal_api.h"


#include "mixer_api.h"
#include "mixer4x1.h"
#include "mixer4x1_api.h"

#include "auto_init_api.h"

char mixer4x1_module_name[] = "mixer4x1";


/**
 * mixer4x1_dsp()
 *
 * return:
 */
void mixer4x1_dsp(struct dsp_module_inst_t *adsp)
{
	struct mixer4x1_instance_t *handle;
	real_t *apCh1In;
	real_t *apCh2In;
	real_t *apCh3In;
	real_t *apCh4In;
	real_t *apCh1Out  ;
	real_t channels_weights_1;
	real_t channels_weights_2;
	real_t channels_weights_3;
	real_t channels_weights_4;
	real_t *channels_weights;
	size_t in_data_len1 ;
	size_t in_data_len2 ;
	size_t in_data_len3 ;
	size_t in_data_len4 ;
	size_t out_data_len ;
	real_t curr_val;

	handle = (struct mixer4x1_instance_t *)adsp->handle;

	dsp_get_input_buffer_from_pad(adsp, 0, &apCh1In, &in_data_len1);
	dsp_get_input_buffer_from_pad(adsp, 1, &apCh2In, &in_data_len2);
	dsp_get_input_buffer_from_pad(adsp, 2, &apCh3In, &in_data_len3);
	dsp_get_input_buffer_from_pad(adsp, 3, &apCh4In, &in_data_len4);
	dsp_get_output_buffer_from_pad(adsp, 0, &apCh1Out, &out_data_len);

	if (in_data_len1 != in_data_len2 )
	{
		CRITICAL_ERROR("bad input buffer size");
	}

	if (in_data_len2 != in_data_len3 )
	{
		CRITICAL_ERROR("bad input buffer size");
	}

	if (in_data_len3 != in_data_len4 )
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
	channels_weights_3 =  channels_weights[2];
	channels_weights_4 =  channels_weights[3];

	while( in_data_len1--)
	{
		curr_val = (*apCh1In++) * channels_weights_1;
		curr_val += (*apCh2In++) * channels_weights_2;
		curr_val += (*apCh3In++) * channels_weights_3;
		curr_val += (*apCh4In++) * channels_weights_4;
		*apCh1Out++ = curr_val;
	}
}





/**
 * mixer4x1_ioctl()
 *
 * return:
 */
uint8_t mixer4x1_ioctl(struct dsp_module_inst_t *adsp,
		const uint8_t aIoctl_num, void * aIoctl_param1, void * aIoctl_param2)
{
	struct mixer4x1_instance_t *handle;
	uint8_t i;
	real_t *channels_weights ;

	handle = (struct mixer4x1_instance_t *)adsp->handle;
	channels_weights = handle->channels_weights ;
	switch(aIoctl_num)
	{
	case IOCTL_DSP_INIT :
		channels_weights[0] = 1.0f;
		channels_weights[1] = 1.0f;
		channels_weights[2] = 1.0f;
		channels_weights[3] = 1.0f;
		break;

	case IOCTL_MIXER_SET_CHANNEL_WEIGHT :
		i = ((struct set_channel_weight_t *)aIoctl_param1)->channel_num;
		if ( 4 > i)
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
 * mixer4x1_init()
 *
 * return:
 */
extern "C" void  mixer4x1_init(void)
{
	DSP_REGISTER_NEW_MODULE("mixer4x1",
			mixer4x1_ioctl , mixer4x1_dsp , struct mixer4x1_instance_t);
}

AUTO_INIT_FUNCTION(mixer4x1_init);
