/*
 *
 * file :   pcm_mixer.c
 *
 *
 */



/********  includes *********************/
#include "_project_typedefs.h"
#include "_project_defines.h"

#include "dsp_management_api.h"
#include "dsp_management_internal_api.h"

#include "pcm_mixer.h"

#include "pcm_mixer_api.h"
#include "auto_init_api.h"
#include <string.h>


/********  defines *********************/

#ifndef ABS
	real_t zero((int16_t)0);
	#define ABS(x)  (x > zero ? x : (zero - x))
#endif
/********  types  *********************/

/********  externals *********************/


/********  exported variables *********************/

char pcm_mixer_module_name[] = "pcm_mixer";


/**********   external variables    **************/



/***********   local variables    **************/


/**
 * pcm_mixer_dsp()
 *
 * return:
 */
static void pcm_mixer_dsp_default(struct dsp_module_inst_t *adsp,
		struct dsp_pad_t *in_pads[MAX_NUM_OF_OUTPUT_PADS] ,
		struct dsp_pad_t out_pads[MAX_NUM_OF_OUTPUT_PADS])
{
	CRITICAL_ERROR("should use another dsp function");
}


static void channel_copy_16bit(real_t *pRxBuf, uint8_t *outChannel,
		size_t num_of_frames, uint8_t frame_size_bytes, real_t normalizer)
{
	size_t i;
	for (i = 0; i < num_of_frames; i++)
	{
		real_t in_real;

		in_real = *pRxBuf++;

		*(int16_t*)outChannel = (int16_t)(in_real * normalizer);
		outChannel += frame_size_bytes;
	}
}


static void channel_copy_32bit(real_t *pRxBuf, uint8_t *outChannel,
		size_t num_of_frames, uint8_t frame_size_bytes, real_t normalizer)
{
	size_t i;
	for (i = 0; i < num_of_frames; i++)
	{
		real_t in_real;

		in_real = *pRxBuf++;
		*(int32_t*)outChannel = (int32_t)(in_real * normalizer);
		outChannel += frame_size_bytes;
	}

}

/**
 * pcm_mixer_dsp_16and32bit()
 *
 * return:
 */
void pcm_mixer_dsp_16and32bit(struct dsp_module_inst_t *adsp,
		struct dsp_pad_t *in_pads[MAX_NUM_OF_OUTPUT_PADS] ,
		struct dsp_pad_t  out_pads[MAX_NUM_OF_OUTPUT_PADS])
{
	real_t *apChIn[MAX_NUM_OF_OUTPUT_PADS];
	size_t in_data_len[MAX_NUM_OF_OUTPUT_PADS] ;
	struct pcm_mixer_instance_t *handle;
	uint8_t enable_test_clipping;
	size_t num_of_frames ;
	uint8_t *pTxBuf;
	size_t i;
	uint8_t num_of_channels ;
	struct pcm_mixer_api_set_params_t *set_params;
	uint8_t subframe_size_bytes;
	uint8_t frame_size_bytes;
	real_t normalizer;
	uint8_t channel_num;

	handle = (struct pcm_mixer_instance_t *)adsp->handle;
	set_params = &handle->set_params;
	num_of_channels = handle->num_of_channels;

	if ((0 == num_of_channels) || (num_of_channels > MAX_NUM_OF_OUTPUT_PADS ))
	{
		CRITICAL_ERROR("unsupported number of channels");
	}

	for (i = 0; i < num_of_channels; i++)
	{
		dsp_get_buffer_from_pad(in_pads[i], &apChIn[i], &in_data_len[i]);
	}
	if (in_data_len[0] != in_data_len[1] )
	{
		CRITICAL_ERROR("bad input buffer size");
	}


	/*
	 * casting here is just to avoid warning as we are aware that
	 * pTxBuf has some INT type
	 */
	dsp_get_buffer_from_pad(&out_pads[0], (real_t**)&pTxBuf, &num_of_frames);

	subframe_size_bytes = set_params->subframe_size_bytes;
	num_of_frames /= ( num_of_channels * subframe_size_bytes);

	if (num_of_frames != in_data_len[0] )
	{
		CRITICAL_ERROR("bad output buffer size");
	}

	frame_size_bytes = set_params->frame_size_bytes;
	normalizer = handle->normalizer;

	for (channel_num = 0; channel_num < num_of_channels; channel_num++)
	{
		real_t *curr_ChIn;

		curr_ChIn = apChIn[channel_num];
		if (2 == subframe_size_bytes)
		{
			channel_copy_16bit(curr_ChIn, pTxBuf,
					num_of_frames, frame_size_bytes, normalizer);
		}
		else if (4 == subframe_size_bytes)
		{
			channel_copy_32bit(curr_ChIn, pTxBuf,
					num_of_frames, frame_size_bytes, normalizer);
		}
		pTxBuf += subframe_size_bytes;
	}
	enable_test_clipping = handle->enable_test_clipping;
	if (enable_test_clipping)
	{
		real_t max_out_val ;
		real_t *apCh1In ;
		real_t *apCh2In ;

		apCh1In = apChIn[0];
		apCh2In = apChIn[1];
		max_out_val = handle->max_out_val;
		for (i = 0; i < num_of_frames; i++)
		{
			real_t tmp1, tmp2;

			tmp1 = ABS(*apCh1In++);
			tmp2 = ABS(*apCh2In++);
			if (tmp1 >= max_out_val)
			{
				max_out_val = tmp1;
			}

			if (tmp2 >= max_out_val)
			{
				max_out_val = tmp2;
			}
		}
		handle->max_out_val = max_out_val;
	}
}


static void set_params(struct dsp_module_inst_t *adsp,
		struct pcm_mixer_instance_t *handle,
		struct pcm_mixer_api_set_params_t *set_params)
{
	uint8_t subframe_size_bytes;
	uint8_t frame_size_bytes;
	uint8_t channel_size_bits;
	real_t normalizer;

	memcpy(&handle->set_params,
			set_params, sizeof(struct pcm_mixer_api_set_params_t));
	subframe_size_bytes = set_params->subframe_size_bytes;
	if ((0 == subframe_size_bytes) || (subframe_size_bytes > 4))
	{
		CRITICAL_ERROR("bad subframe size");
	}

	frame_size_bytes = set_params->frame_size_bytes;
	if (0 == frame_size_bytes)
	{
		CRITICAL_ERROR("bad frame size should not be 0");
	}
	if (0 != (frame_size_bytes % subframe_size_bytes))
	{
		CRITICAL_ERROR("bad frame size should be divided by subframe size");
	}

	handle->num_of_channels = frame_size_bytes / subframe_size_bytes;

	channel_size_bits = set_params->channel_size_bits;
	if (channel_size_bits != (subframe_size_bytes * 8))
	{
		CRITICAL_ERROR("not supported channel size 1");
	}
	if (16 == channel_size_bits)
	{
		normalizer = (float)0x7fff;
		adsp->dsp_func = pcm_mixer_dsp_16and32bit;
	}
	else if (32 == channel_size_bits)
	{
		normalizer = (float)0x7fffffff;
		adsp->dsp_func = pcm_mixer_dsp_16and32bit;
	}
	else
	{
		CRITICAL_ERROR("not supported channel size 2");
	}
	handle->normalizer = normalizer;

	if (PCM_MIXER_CHANNEL_BITS_ARE_LEFT_JUSTIFIED
						!= set_params->channel_justification)
	{
		CRITICAL_ERROR("not supported justification");
	}
}


/**
 * pcm_mixer_ioctl()
 *
 * return:
 */
uint8_t pcm_mixer_ioctl(struct dsp_module_inst_t *adsp,
		uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2)
{
	struct pcm_mixer_instance_t *handle;

	handle = (struct pcm_mixer_instance_t *)adsp->handle;

	switch(aIoctl_num)
	{
	case IOCTL_DSP_INIT :
		handle->enable_test_clipping = 0;
		handle->max_out_val = (int16_t) 0;
		break;

	case IOCTL_PCM_MIXER_ENABLE_TEST_CLIPPING :
		handle->enable_test_clipping = 1;
		break;

	case IOCTL_PCM_MIXER_DISABLE_TEST_CLIPPING :
		handle->enable_test_clipping = 0;
		break;

	case IOCTL_PCM_MIXER_GET_MAX_OUTPUT_VALUE :
		*(real_t*)aIoctl_param1 = handle->max_out_val  ;
		handle->max_out_val = (int16_t) 0;
		break;
	case IOCTL_PCM_MIXER_SET_PARAMS :
		set_params(adsp,
				handle, (struct pcm_mixer_api_set_params_t *)aIoctl_param1);
		break;

	default :
		return 1;
	}
	return 0;
}



void  pcm_mixer_init(void)
{
	DSP_REGISTER_NEW_MODULE("pcm_mixer",
		pcm_mixer_ioctl , pcm_mixer_dsp_default, struct pcm_mixer_instance_t);
}

AUTO_INIT_FUNCTION(pcm_mixer_init);
