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
#include "common_dsp_api.h"


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
void dpwm_mixer_dsp(struct dsp_desc_t *adsp , size_t data_len ,
		struct dsp_pad_t *in_pads[MAX_NUM_OF_OUTPUT_PADS] ,
		struct dsp_pad_t  out_pads[MAX_NUM_OF_OUTPUT_PADS])
{
	float *apCh1In ,  *apCh2In;
	struct DPWM_MIXER_Instance_t *handle;
	uint8_t enable_test_clipping;
	float max_out_val ;
	float *pTxBuf;

	handle = adsp->handle;

	max_out_val = handle->max_out_val;
	enable_test_clipping = handle->enable_test_clipping;


	pTxBuf = out_pads[0].buff;

	apCh1In = in_pads[0]->buff;
	apCh2In = in_pads[1]->buff;


	for( ; data_len ;data_len--)
	{
		if(enable_test_clipping)
		{
			float tmp1, tmp2;
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

		*pTxBuf = (*apCh1In++);
		pTxBuf++;

		*pTxBuf = (*apCh2In++);
		pTxBuf++;


	}

	handle->max_out_val = max_out_val;


}





/**
 * dpwm_mixer_ioctl()
 *
 * return:
 */
uint8_t dpwm_mixer_ioctl(struct dsp_desc_t *adsp,
		uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2)
{
	struct DPWM_MIXER_Instance_t *handle;

	handle = adsp->handle;

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
