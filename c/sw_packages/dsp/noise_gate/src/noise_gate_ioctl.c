/*
 *
 * file :   noise_gate.c
 *

 *
 */



/********  includes *********************/

#include "_noise_gate_prerequirements_check.h"

#include "noise_gate_api.h"
#include "noise_gate.h"

#include "math.h"

#include "PRINTF_api.h"


#include "auto_init_api.h"

/********  defines *********************/

/********  types  *********************/

/********  externals *********************/

/********  exported variables *********************/

char noise_gate_module_name[] = "noise_gate";


/**********   external variables    **************/

/***********   local variables    **************/



/*
 *  val = 1- e ^ (-1/(SR*t))
 */
static float convert_ms_to_non_unit(float ms)
{
	float tmp;

	tmp = -( 1.0f / (48000.0f * ms));
	tmp = exp(tmp);
	tmp = 1.0f - tmp;
	return tmp;
}


/**
 * noise_gate_ioctl()
 *
 * return:
 */
uint8_t noise_gate_ioctl(struct dsp_desc_t *adsp,
		uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2)
{
	struct std_compressor_instance_t *handle;
	float alpha;

	handle = adsp->handle;
	switch(aIoctl_num)
	{
	case IOCTL_DSP_INIT :
		handle->ratio_env_follower = 1.0f;
		handle->rms = 0.0f;
		handle->release = 0.5f;
		handle->attack = 0.5f;
		handle->threshold = 0.99999f;

		alpha = 0.96f;
		handle->alpha =alpha;
		/*
		 *  division by 4 put here instead of
		 *  division by 2 in mono->stereo converter
		 */
		handle->one_minus_alpha = ((1.0f - alpha) / 4);

		break;
	case IOCTL_NOISE_GATE_SET_THRESHOLD_DB :
		handle->threshold = pow(10, (*((float*)aIoctl_param1)) / 20.0f);
		break;
	case IOCTL_NOISE_GATE_SET_ATTACK_mS :
		handle->attack = convert_ms_to_non_unit(*((float*)aIoctl_param1));
		break;
	case IOCTL_NOISE_GATE_SET_RELEASE_mS :
		handle->release = convert_ms_to_non_unit(*((float*)aIoctl_param1));
		break;
	case IOCTL_NOISE_GATE_SET_ALPHA :
		alpha = convert_ms_to_non_unit(*((float*)aIoctl_param1));
		handle->alpha = alpha;
		/*
		 *  division by 4 put here instead of
		 *  division by 2 in mono->stereo converter
		 */
		handle->one_minus_alpha = ((1.0f - alpha) / 4.0f);
		break;
	default :
		return 1;
	}
	return 0;
}


extern void noise_gate_dsp(struct dsp_desc_t *adsp, size_t data_len,
		struct dsp_pad_t *in_pads[MAX_NUM_OF_OUTPUT_PADS] ,
		struct dsp_pad_t out_pads[MAX_NUM_OF_OUTPUT_PADS]);


void  noise_gate_init(void)
{
	DSP_REGISTER_NEW_MODULE(NOISE_GATE_API_MODULE_NAME,
			noise_gate_ioctl, noise_gate_dsp,
			struct std_compressor_instance_t);
}

AUTO_INIT_FUNCTION(noise_gate_init);

