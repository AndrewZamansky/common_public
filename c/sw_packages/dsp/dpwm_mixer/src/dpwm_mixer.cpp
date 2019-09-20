/*
 *
 * file :   dpwm_mixer.c
 *
 *
 */



/********  includes *********************/
#include "_project_typedefs.h"
#include "_project_defines.h"

#include "dsp_management_api.h"
#include "dsp_management_internal_api.h"


#include "dpwm_mixer_api.h"
#include "dpwm_mixer.h"

#include "auto_init_api.h"

#include "_dpwm_mixer_prerequirements_check.h"

/********  defines *********************/
#ifndef ABS
#define ABS(x)  (x > 0 ? x : -x)
#endif

/********  types  *********************/

/********  externals *********************/


/********  exported variables *********************/

char dpwm_mixer_module_name[] = "dpwm_mixer";


/**********   external variables    **************/



/***********   local variables    **************/


/**
 * dpwm_mixer_dsp()
 *
 * return:
 */
void dpwm_mixer_dsp(struct dsp_module_inst_t *adsp)
{
	real_t *apCh1In;
	real_t *apCh2In;
	size_t in_data_len1 ;
	size_t in_data_len2 ;
	size_t out_data_len ;
	struct DPWM_MIXER_Instance_t *handle;
	uint8_t enable_test_clipping;
	real_t max_out_val ;
	real_t *pTxBuf;
	real_t inVal1;
	real_t inVal2;
	uint32_t i;

	handle = (struct DPWM_MIXER_Instance_t *)adsp->handle;

	dsp_get_input_buffer_from_pad(adsp, 0, &apCh1In, &in_data_len1);
	dsp_get_input_buffer_from_pad(adsp, 1, &apCh2In, &in_data_len2);
	dsp_get_output_buffer_from_pad(adsp, 0, &pTxBuf, &out_data_len);

	if (in_data_len1 != in_data_len2 )
	{
		CRITICAL_ERROR("bad input buffer size");
	}

	if (out_data_len < (in_data_len1 + in_data_len2) )
	{
		CRITICAL_ERROR("output buffer is too small");
	}

	max_out_val = handle->max_out_val;
	enable_test_clipping = handle->enable_test_clipping;


	inVal1 = 0;
	inVal2 = 0;

	for ( i = 0; i < in_data_len1; i++)
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
uint8_t dpwm_mixer_ioctl(struct dsp_module_inst_t *adsp,
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



void  dpwm_mixer_init(void)
{
	DSP_REGISTER_NEW_MODULE("dpwm_mixer",
			dpwm_mixer_ioctl , dpwm_mixer_dsp , struct DPWM_MIXER_Instance_t);
}

AUTO_INIT_FUNCTION(dpwm_mixer_init);
