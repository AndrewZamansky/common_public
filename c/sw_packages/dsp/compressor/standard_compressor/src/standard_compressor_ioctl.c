/*
 *
 * file :   standard_compressor.c
 *
 *
 *
 *
 *
 */



/********  includes *********************/

#include "_standard_compressor_prerequirements_check.h"

#include "standard_compressor_api.h"
#include "standard_compressor.h"

#include "math.h"

#include "PRINTF_api.h"


#include "auto_init_api.h"

/********  defines *********************/

/********  types  *********************/

/********  externals *********************/

/********  exported variables *********************/

char standard_compressor_module_name[] = "standard_compressor";


/**********   external variables    **************/

/***********   local variables    **************/



/*
 *  val = 1- e ^ (-1/(SR*t))
 */
static float convert_ms_to_non_unit(float ms)
{
	float tmp;

	ms = ms / 1000; //transfer to seconds
	tmp = -( 1.0f / (48000.0f * ms));
	tmp = exp(tmp);
	tmp = 1.0f - tmp;
	return tmp;
}


/**
 * standard_compressor_ioctl()
 *
 * return:
 */
uint8_t standard_compressor_ioctl(struct dsp_desc_t *adsp,
		uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2)
{
	struct std_compressor_instance_t *handle;
	float alpha;
	float tmp;
	struct standard_compressor_api_set_params_t *set_params;

	handle = adsp->handle;
	set_params = &handle->set_params;
	switch(aIoctl_num)
	{
	case IOCTL_DSP_INIT :
		handle->reverse_ratio = 1.0f;//0.5;
		handle->ratio_env_follower = 1.0f;
		set_params->ratio = 1;

		handle->rms = 0.0f;
		handle->release = 0.5f;
		set_params->release = 0.03;

		handle->attack = 0.5f;
		set_params->attack = 0.03;

		set_params->threshold = 0;
		handle->threshold = 0.99999f;

		set_params->gain = 0;
		handle->gain =1.0f;

		alpha = 0.96f;
		handle->alpha =alpha;
		set_params->alpha = 0.00647;
		/*
		 *  division by 4 put here instead of
		 *  division by 2 in mono->stereo converter
		 */
		handle->one_minus_alpha = ((1.0f - alpha) / 4);

		break;
	case IOCTL_STANDARD_COMPRESSOR_SET_GAIN_DB :
		tmp = *((float*)aIoctl_param1);
		set_params->gain = tmp;
		handle->gain = pow(10, tmp / 20.0f);
		break;
	case IOCTL_STANDARD_COMPRESSOR_SET_HIGH_THRESHOLD_DB :
		tmp = *((float*)aIoctl_param1);
		set_params->threshold = tmp;
		handle->threshold = pow(10, tmp / 20.0f);
		break;
	case IOCTL_STANDARD_COMPRESSOR_SET_RATIO :
		tmp = *((float*)aIoctl_param1);
		set_params->ratio = tmp;
		handle->reverse_ratio = 1/tmp;
		break;
	case IOCTL_STANDARD_COMPRESSOR_SET_ATTACK_mS :
		tmp = *((float*)aIoctl_param1);
		set_params->attack = tmp;
		handle->attack = convert_ms_to_non_unit(tmp);
		break;
	case IOCTL_STANDARD_COMPRESSOR_SET_RELEASE_mS :
		tmp = *((float*)aIoctl_param1);
		set_params->release = tmp;
		handle->release = convert_ms_to_non_unit(tmp);
		break;
	case IOCTL_STANDARD_COMPRESSOR_SET_ALPHA :
		tmp = *((float*)aIoctl_param1);
		set_params->alpha = tmp;
		alpha = convert_ms_to_non_unit(tmp);
		handle->alpha = alpha;
		/*
		 *  division by 4 put here instead of
		 *  division by 2 in mono->stereo converter
		 */
		handle->one_minus_alpha = ((1.0f - alpha) / 4.0f);
		break;
	case IOCTL_STANDARD_COMPRESSOR_GET_PARAMS:
		memcpy(aIoctl_param1, set_params,
				sizeof(struct standard_compressor_api_set_params_t));
		break;
	default :
		return 1;
	}
	return 0;
}


extern void standard_compressor_dsp(struct dsp_desc_t *adsp, size_t data_len,
		struct dsp_pad_t *in_pads[MAX_NUM_OF_OUTPUT_PADS] ,
		struct dsp_pad_t out_pads[MAX_NUM_OF_OUTPUT_PADS]);


void  standard_compressor_init(void)
{
	DSP_REGISTER_NEW_MODULE(STANDARD_COMPRESSOR_API_MODULE_NAME,
			standard_compressor_ioctl, standard_compressor_dsp,
			struct std_compressor_instance_t);
}

AUTO_INIT_FUNCTION(standard_compressor_init);

