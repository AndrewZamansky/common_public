/*
 *
 * file :   multiplier_1ch.c
 *
 *
 */
#include "_project_typedefs.h"
#include "_project_defines.h"

#include "dsp_management_api.h"
#include "dsp_management_internal_api.h"

#include "multiplier_api.h"
#include "multiplier_1ch_api.h"
#include "multiplier_1ch.h"
#include "auto_init_api.h"
#include "string.h"
#include "math.h"

#define ln_of_10   2.302585092994f
#define _20_div_ln_of_10   8.685889638065f

char multiplier_1ch_module_name[] = "multiplier_1ch";

/**
 * multiplier_1ch_dsp()
 *
 * return:
 */
static void multiplier_1ch_dsp(struct dsp_module_inst_t *adsp)
{
	real_t *apCh1In;
	real_t *apCh1Out;
	struct multiplier_1ch_instance_t *handle;
	real_t weight;
	real_t curr_val;
	size_t data_len;
	uint8_t buff_is_zero_buffer;

	buff_is_zero_buffer = 1;
	dsp_get_input_buffer_from_pad(
			adsp, 0, (uint8_t**)&apCh1In, &data_len, &buff_is_zero_buffer);
	dsp_get_output_buffer_from_pad(adsp, 0, (uint8_t**)&apCh1Out, data_len);

	handle = (struct multiplier_1ch_instance_t *)adsp->handle;
	weight = handle->weight;
	data_len = data_len / sizeof(real_t);
	while (data_len--)
	{
		curr_val = (*apCh1In++) * weight;
		*apCh1Out++ = curr_val;
	}
}


/**
 * multiplier_1ch_ioctl()
 *
 * return:
 */
static uint8_t multiplier_1ch_ioctl(struct dsp_module_inst_t *adsp,
		const uint8_t aIoctl_num, void * aIoctl_param1 , void * aIoctl_param2)
{
	real_t weight_db;
	real_t weight;
	real_t tmp;
	struct multiplier_1ch_instance_t *handle;
	struct multiplier_api_set_params_t *set_params;

	handle = (struct multiplier_1ch_instance_t *)adsp->handle;
	set_params = &handle->set_params;

	switch(aIoctl_num)
	{
	case IOCTL_DSP_INIT :
		handle->weight = (int16_t)1;
		set_params->weight_db = (int16_t)0;
		break;

	case IOCTL_MULTIPLIER_SET_WEIGHT_DB :
		weight_db = *(float*)aIoctl_param1;
		tmp = (float)10;
		#ifdef CONFIG_DSP_REAL_NUMBER_FORMAT_FLOATING_POINT
			handle->weight = pow(tmp, weight_db/(float)20);
		#else
			tmp = fix16_log(tmp);
			tmp *= (weight_db / (float)20);
			handle->weight = fix16_exp(tmp);
		#endif
		set_params->weight_db = weight_db;
		break;

	case IOCTL_MULTIPLIER_SET_WEIGHT :
		weight = *(float*)aIoctl_param1;
		#ifdef CONFIG_DSP_REAL_NUMBER_FORMAT_FLOATING_POINT
			weight_db = log10(weight) * (float)20;
		#else
			weight_db = fix16_log(weight) * _20_div_ln_of_10;
		#endif
		set_params->weight_db = weight_db;
		handle->weight = weight;
		break;

	case IOCTL_MULTIPLIER_GET_PARAMS :
		memcpy(aIoctl_param1, set_params,
				sizeof(struct multiplier_api_set_params_t));
		break;
	default :
			return 1;
	}
	return 0;
}


/**
 * multiplier_1ch_init()
 *
 * return:
 */
extern "C" void  multiplier_1ch_init(void)
{
	DSP_REGISTER_NEW_MODULE("multiplier_1ch", multiplier_1ch_ioctl,
			multiplier_1ch_dsp,
			dsp_management_default_bypass, dsp_management_default_mute,
			0, struct multiplier_1ch_instance_t);
}

AUTO_INIT_FUNCTION(multiplier_1ch_init);
