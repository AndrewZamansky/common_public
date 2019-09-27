/*
 *
 * file :   pcm_splitter.c
 *
 */



/********  includes *********************/
#include "_project_typedefs.h"
#include "_project_defines.h"

#include "dsp_management_api.h"
#include "dsp_management_internal_api.h"


#include "pcm_splitter_api.h"
#include "auto_init_api.h"

#include "pcm_splitter.h"
#include <string.h>

/********  defines *********************/

/********  types  *********************/

/********  externals *********************/


/********  exported variables *********************/

char pcm_splitter_module_name[] = "pcm_splitter";


/**********   external variables    **************/



/***********   local variables    **************/


/**
 * pcm_splitter_dsp()
 *
 * return:
 */
static void pcm_splitter_dsp_default(struct dsp_module_inst_t *adsp,
		struct dsp_pad_t *in_pads[MAX_NUM_OF_OUTPUT_PADS] ,
		struct dsp_pad_t out_pads[MAX_NUM_OF_OUTPUT_PADS])
{
	CRITICAL_ERROR("should use another dsp function");
}

static void channel_copy_16bit(uint8_t *pRxBuf, real_t *outChannel,
		size_t num_of_frames, uint8_t frame_size_bytes, real_t normalizer)
{
	size_t i;
	for (i = 0; i < num_of_frames; i++)
	{
		real_t in_real;

		in_real = *(int16_t*)pRxBuf;
		*outChannel++ = in_real * normalizer;
		pRxBuf += frame_size_bytes;
	}
}


static void channel_copy_32bit(uint8_t *pRxBuf, real_t *outChannel,
		size_t num_of_frames, uint8_t frame_size_bytes)
{
	size_t i;
	real_t normalizer;

	normalizer = 0x7fffffff;
	for (i = 0; i < num_of_frames; i++)
	{
		real_t in_real;

		in_real = *(int32_t*)pRxBuf;
		*outChannel++ = in_real / normalizer;
		pRxBuf += frame_size_bytes;
	}
}

/**
 * pcm_splitter_dsp_16and32bit()
 *
 * return:
 */
static void pcm_splitter_dsp_16and32bit(struct dsp_module_inst_t *adsp,
		struct dsp_pad_t *in_pads[MAX_NUM_OF_OUTPUT_PADS] ,
		struct dsp_pad_t out_pads[MAX_NUM_OF_OUTPUT_PADS])
{
	real_t *apChOut[MAX_NUM_OF_OUTPUT_PADS];
	size_t num_of_frames;
	size_t out_data_len[MAX_NUM_OF_OUTPUT_PADS] ;
	uint8_t *pRxBuf;
	uint8_t num_of_channels ;
	struct pcm_splitter_instance_t *handle;
	uint8_t i;
	struct pcm_splitter_api_set_params_t *set_params;
	uint8_t subframe_size_bytes;
	uint8_t frame_size_bytes;
	real_t normalizer;
	uint8_t channel_num;

	handle = (struct pcm_splitter_instance_t *)adsp->handle;
	set_params = &handle->set_params;
	num_of_channels = handle->num_of_channels;

	if ((0 == num_of_channels) || (num_of_channels > MAX_NUM_OF_OUTPUT_PADS ))
	{
		CRITICAL_ERROR("unsupported number of channels");
	}

	for (i = 0; i < num_of_channels; i++)
	{
		dsp_get_buffer_from_pad(&out_pads[i], &apChOut[i], &out_data_len[i]);
		if (out_data_len[0] != out_data_len[i] )
		{
			CRITICAL_ERROR("bad output buffer size");
		}
	}

	subframe_size_bytes = set_params->subframe_size_bytes;
	/*
	 * casting here is just to avoid warning as we are aware that
	 * pRxBuf has some INT type
	 */
	dsp_get_buffer_from_pad(in_pads[0], (real_t**)&pRxBuf, &num_of_frames);

	num_of_frames /= ( num_of_channels * subframe_size_bytes);
	if (num_of_frames != out_data_len[0] )
	{
		CRITICAL_ERROR("bad input buffer size");
	}

	frame_size_bytes = set_params->frame_size_bytes;
	normalizer = handle->normalizer;
	for (channel_num = 0; channel_num < num_of_channels; channel_num++)
	{
		real_t *curr_ChOut;

		curr_ChOut = apChOut[channel_num];
		if (2 == subframe_size_bytes)
		{
			channel_copy_16bit(pRxBuf, curr_ChOut,
					num_of_frames, frame_size_bytes, normalizer);
		}
		else if (4 == subframe_size_bytes)
		{
			channel_copy_32bit(pRxBuf, curr_ChOut,
					num_of_frames, frame_size_bytes);
		}
		pRxBuf += subframe_size_bytes;
	}
}




static void set_params(struct dsp_module_inst_t *adsp,
		struct pcm_splitter_instance_t *handle,
		struct pcm_splitter_api_set_params_t *set_params)
{
	uint8_t subframe_size_bytes;
	uint8_t frame_size_bytes;
	uint8_t channel_size_bits;
	real_t normalizer;
	uint8_t num_of_channels;

	memcpy(&handle->set_params,
			set_params, sizeof(struct pcm_splitter_api_set_params_t));
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

	num_of_channels = frame_size_bytes / subframe_size_bytes;
	if (4 < num_of_channels)
	{
		CRITICAL_ERROR("not supported number of channels");
	}
	handle->num_of_channels = num_of_channels;

	channel_size_bits = set_params->channel_size_bits;
	if (channel_size_bits != (subframe_size_bytes * 8))
	{
		CRITICAL_ERROR("not supported channel size 1");
	}
	if (16 == channel_size_bits)
	{
		normalizer = (float)(1.0f / (float)0x7fff);
		adsp->dsp_func = pcm_splitter_dsp_16and32bit;
	}
	else if (32 == channel_size_bits)
	{
		normalizer = (float)(1.0f / (float)0x7fffffff);
		adsp->dsp_func = pcm_splitter_dsp_16and32bit;
	}
	else
	{
		normalizer = 1;
		CRITICAL_ERROR("not supported channel size 2");
	}
	handle->normalizer = normalizer;

	if (PCM_SPLITTER_CHANNEL_BITS_ARE_LEFT_JUSTIFIED
						!= set_params->channel_justification)
	{
		CRITICAL_ERROR("not supported justification");
	}
}


/**
 * pcm_splitter_ioctl()
 *
 * return:
 */
uint8_t pcm_splitter_ioctl(struct dsp_module_inst_t *adsp,
		uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2)
{
	struct pcm_splitter_instance_t *handle;

	handle = (struct pcm_splitter_instance_t *)adsp->handle;

	switch(aIoctl_num)
	{
	case IOCTL_DSP_INIT :
		memset(handle, 0, sizeof(struct pcm_splitter_instance_t));
		break;
	case IOCTL_PCM_SPLITTER_SET_PARAMS :
		set_params(adsp,
				handle, (struct pcm_splitter_api_set_params_t *)aIoctl_param1);
		break;

	default :
		return 1;
	}
	return 0;
}




void  pcm_splitter_init(void)
{
	DSP_REGISTER_NEW_MODULE("pcm_splitter",	pcm_splitter_ioctl,
			pcm_splitter_dsp_default , struct pcm_splitter_instance_t);
}

AUTO_INIT_FUNCTION(pcm_splitter_init);
