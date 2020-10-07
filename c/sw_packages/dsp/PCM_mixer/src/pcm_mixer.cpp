/*
 *
 * file :   pcm_mixer.c
 *
 *
 */
#include "_project_typedefs.h"
#include "_project_defines.h"

#include "dsp_management_api.h"
#include "dsp_management_internal_api.h"

#include "pcm_mixer.h"

#include "pcm_mixer_api.h"
#include "auto_init_api.h"
#include <string.h>



#ifndef ABS
	real_t zero((int16_t)0);
	#define ABS(x)  (x > zero ? x : (zero - x))
#endif

char pcm_mixer_module_name[] = "pcm_mixer";



static void channel_copy_16bit(real_t *pRxBuf, uint8_t *outChannel,
		size_t num_of_frames, uint8_t frame_size_bytes)
{
	size_t i;
	real_t normalizer;

	normalizer = (float)0x7fff;
	for (i = 0; i < num_of_frames; i++)
	{
		real_t in_real;

		in_real = *pRxBuf++;

		*(int16_t*)outChannel = (int16_t)(in_real * normalizer);
		outChannel += frame_size_bytes;
	}
}


static void channel_copy_24bit(real_t *pRxBuf, uint8_t *outChannel,
		size_t num_of_frames, uint8_t frame_size_bytes)
{
	size_t i;
	real_t normalizer;

	normalizer = (float)0x7fffffff;
	for (i = 0; i < num_of_frames; i++)
	{
		real_t in_real;
		uint32_t val;

		in_real = *pRxBuf++;
		val = (uint32_t)((int32_t)(in_real * normalizer));
		val = val >> 8; // skip lowest LSB 8 bits
		outChannel[0] = val & 0xFF;
		val = val >> 8;
		outChannel[1] = val & 0xFF;
		val = val >> 8;
		outChannel[2] = val & 0xFF;
		outChannel += frame_size_bytes;
	}
}


static void channel_copy_32bit(real_t *pRxBuf, uint8_t *outChannel,
		size_t num_of_frames, uint8_t frame_size_bytes)
{
	size_t i;
	real_t normalizer;

	normalizer = (float)0x7fffffff;
	for (i = 0; i < num_of_frames; i++)
	{
		real_t in_real;

		in_real = *pRxBuf++;
		*(int32_t*)outChannel = (int32_t)(in_real * normalizer);
		outChannel += frame_size_bytes;
	}

}

#define MAX_NUM_OF_OUTPUT_PADS  4
/**
 * pcm_mixer_dsp_16and32bit()
 *
 * return:
 */
static void pcm_mixer_dsp_func(struct dsp_module_inst_t *adsp)
{
	real_t *apChIn[MAX_NUM_OF_OUTPUT_PADS];
	size_t data_len;
	struct pcm_mixer_instance_t *handle;
	uint8_t enable_test_clipping;
	size_t num_of_frames ;
	uint8_t *pTxBuf;
	uint8_t num_of_channels ;
	struct pcm_mixer_api_set_params_t *set_params;
	uint8_t subframe_size_bytes;
	uint8_t frame_size_bytes;
	uint8_t channel_num;
	uint8_t buff_is_zero_buffer;

	handle = (struct pcm_mixer_instance_t *)adsp->handle;
	set_params = &handle->set_params;
	num_of_channels = handle->num_of_channels;

	if ((0 == num_of_channels) || (num_of_channels > MAX_NUM_OF_OUTPUT_PADS ))
	{
		CRITICAL_ERROR("unsupported number of channels");
	}

	buff_is_zero_buffer = 1;
	data_len = 0;
	for (uint8_t j = 0; j < num_of_channels; j++)
	{
		dsp_get_input_buffer_from_pad(
				adsp, j, &(uint8_t*)apChIn[j], &data_len, &buff_is_zero_buffer);
	}


	subframe_size_bytes = set_params->subframe_size_bytes;
	num_of_frames = data_len / sizeof(real_t);
	dsp_get_output_buffer_from_pad(adsp, 0, &pTxBuf,
			num_of_frames * subframe_size_bytes * num_of_channels);

	frame_size_bytes = set_params->frame_size_bytes;
	for (channel_num = 0; channel_num < num_of_channels; channel_num++)
	{
		real_t *curr_ChIn;

		curr_ChIn = apChIn[channel_num];
		if (2 == subframe_size_bytes)
		{
			channel_copy_16bit(curr_ChIn, pTxBuf,
					num_of_frames, frame_size_bytes);
		}
		else if (3 == subframe_size_bytes)
		{
			channel_copy_24bit(curr_ChIn, pTxBuf,
					num_of_frames, frame_size_bytes);
		}
		else if (4 == subframe_size_bytes)
		{
			channel_copy_32bit(curr_ChIn, pTxBuf,
					num_of_frames, frame_size_bytes);
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
		for (size_t i = 0; i < num_of_frames; i++)
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
		CRITICAL_ERROR("supported only fully occupied channel");
	}
	if ((16 != channel_size_bits) &&
			(32 != channel_size_bits) && (24 != channel_size_bits))
	{
		CRITICAL_ERROR("not supported channel size");
	}

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
static uint8_t pcm_mixer_ioctl(struct dsp_module_inst_t *adsp,
		uint8_t aIoctl_num, void * aIoctl_param1, void * aIoctl_param2)
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


extern "C" void  pcm_mixer_init(void)
{
	DSP_REGISTER_NEW_MODULE("pcm_mixer", pcm_mixer_ioctl, pcm_mixer_dsp_func,
		NULL, dsp_management_default_mute,
		DSP_MANAGEMENT_FLAG_BYPASS_NOT_AVAILABLE, struct pcm_mixer_instance_t);
}

AUTO_INIT_FUNCTION(pcm_mixer_init);
