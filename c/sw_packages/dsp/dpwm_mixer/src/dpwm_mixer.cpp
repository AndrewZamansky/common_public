/*
 *
 * file :   dpwm_mixer.c
 *
 *
 */
#include "_project_typedefs.h"
#include "_project_defines.h"

#include "dsp_management_api.h"
#include "dsp_management_internal_api.h"
#include <string.h>

#include "dpwm_mixer_api.h"
#include "dpwm_mixer.h"

#include "auto_init_api.h"


#ifndef ABS
#define ABS(x)  (x > zero ? x : (zero - x))
#endif

char dpwm_mixer_module_name[] = "dpwm_mixer";

static 	real_t zero = 0.0f;

static void dpwm_mixer_mute(struct dsp_module_inst_t * adsp)
{
	real_t *apCh1In;
	real_t *apCh2In;
	size_t data_len;
	size_t out_data_len;
	float *pTxBuf;
	uint8_t buff_is_zero_buffer;

	buff_is_zero_buffer = 1;
	dsp_get_input_buffer_from_pad(
			adsp, 0, (uint8_t**)&apCh1In, &data_len, &buff_is_zero_buffer);
	dsp_get_input_buffer_from_pad(
			adsp, 1, (uint8_t**)&apCh2In, &data_len, &buff_is_zero_buffer);

	out_data_len = 2 * (sizeof(float) * (data_len / sizeof(real_t)));
	dsp_get_output_buffer_from_pad(adsp, 0, (uint8_t**)&pTxBuf, out_data_len);

	memset(pTxBuf, 0, out_data_len);
}


/**
 * dpwm_mixer_dsp()
 *
 * return:
 */
static void dpwm_mixer_dsp(struct dsp_module_inst_t *adsp)
{
	real_t *apCh1In;
	real_t *apCh2In;
	size_t data_len;
	size_t out_data_len ;
	struct DPWM_MIXER_Instance_t *handle;
	uint8_t enable_test_clipping;
	real_t max_out_val ;
	float *pTxBuf;
	real_t inVal1;
	real_t inVal2;
	uint32_t i;
	uint8_t buff_is_zero_buffer;

	buff_is_zero_buffer = 1;
	dsp_get_input_buffer_from_pad(
			adsp, 0, (uint8_t**)&apCh1In, &data_len, &buff_is_zero_buffer);
	dsp_get_input_buffer_from_pad(
			adsp, 1, (uint8_t**)&apCh2In, &data_len, &buff_is_zero_buffer);

	out_data_len = 2 * (sizeof(float) * (data_len / sizeof(real_t)));
	dsp_get_output_buffer_from_pad(adsp, 0, (uint8_t**)&pTxBuf, out_data_len);

	handle = (struct DPWM_MIXER_Instance_t *)adsp->handle;
	max_out_val = handle->max_out_val;
	enable_test_clipping = handle->enable_test_clipping;


	inVal1 = zero;
	inVal2 = zero;

	data_len = data_len / sizeof(real_t);
	for ( i = 0; i < data_len; i++)
	{
		if(enable_test_clipping)
		{
			real_t tmp1, tmp2;
			tmp1 = ABS(*apCh1In);
			tmp2 = ABS(*apCh2In);
			if (tmp1 >= max_out_val)
			{
				max_out_val = tmp1;
			}

			if (tmp2>= max_out_val)
			{
				max_out_val = tmp2;
			}
		}

		inVal1 = (*apCh1In++);
		*pTxBuf = inVal1;
		pTxBuf++;

		inVal2 = (*apCh2In++);
		*pTxBuf = inVal2;
		pTxBuf++;

	}

	handle->max_out_val = max_out_val;
}



/**
 * dpwm_mixer_ioctl()
 *
 * return:
 */
static uint8_t dpwm_mixer_ioctl(struct dsp_module_inst_t *adsp,
		uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2)
{
	struct DPWM_MIXER_Instance_t *handle;

	handle = (struct DPWM_MIXER_Instance_t *)adsp->handle;

	switch(aIoctl_num)
	{
		case IOCTL_DSP_INIT :
			handle->enable_test_clipping = 0;
			handle->max_out_val = 0;
			break;

		case IOCTL_DPWM_MIXER_ENABLE_TEST_CLIPPING :
			handle->enable_test_clipping = 1;
			break;

		case IOCTL_DPWM_MIXER_DISABLE_TEST_CLIPPING :
			handle->enable_test_clipping = 0;
			break;

		case IOCTL_DPWM_MIXER_GET_MAX_OUTPUT_VALUE :
			*(float*)aIoctl_param1 = handle->max_out_val  ;
			handle->max_out_val = 0;
			break;

		default :
			return 1;
	}
	return 0;
}


extern "C" void  dpwm_mixer_init(void)
{
	DSP_REGISTER_NEW_MODULE("dpwm_mixer", dpwm_mixer_ioctl, dpwm_mixer_dsp,
		NULL, dpwm_mixer_mute,
		DSP_MANAGEMENT_FLAG_BYPASS_NOT_AVAILABLE, struct DPWM_MIXER_Instance_t);
}

AUTO_INIT_FUNCTION(dpwm_mixer_init);
