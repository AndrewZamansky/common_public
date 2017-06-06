/*
 *
 * file :   I2S_mixer.c
 *
 *
 */



/********  includes *********************/
#include "_project_typedefs.h"
#include "_project_defines.h"

#include "dsp_management_api.h"
#include "common_dsp_api.h"


#include "I2S_mixer_api.h"
#include "I2S_mixer.h"

#include "auto_init_api.h"

#include "_I2S_mixer_prerequirements_check.h"

/********  defines *********************/
#if (2==NUM_OF_BYTES_PER_AUDIO_WORD)
	#define	FLOAT_NORMALIZER	0x7fff
	typedef int16_t	buffer_type_t	;
#endif
#if (4==NUM_OF_BYTES_PER_AUDIO_WORD)
	#define	FLOAT_NORMALIZER	0x7fffffff
	typedef int32_t	buffer_type_t	;
#endif

#ifndef ABS
#define ABS(x)  (x > 0 ? x : -x)
#endif
/********  types  *********************/

/********  externals *********************/


/********  exported variables *********************/

char I2S_mixer_module_name[] = "I2S_mixer";


/**********   external variables    **************/



/***********   local variables    **************/


/**
 * I2S_mixer_dsp()
 *
 * return:
 */
void I2S_mixer_dsp(struct dsp_desc_t *adsp,
		struct dsp_pad_t *in_pads[MAX_NUM_OF_OUTPUT_PADS] ,
		struct dsp_pad_t  out_pads[MAX_NUM_OF_OUTPUT_PADS])
{
	float *apCh1In;
	float *apCh2In;
	struct I2S_MIXER_Instance_t *handle;
	uint8_t enable_test_clipping;
	float max_out_val ;
	float out_val ;
	size_t in_data_len1 ;
	size_t in_data_len2 ;
	size_t out_data_len ;
	buffer_type_t *pTxBuf;

	handle = adsp->handle;

	max_out_val = handle->max_out_val;
	enable_test_clipping = handle->enable_test_clipping;

	DSP_GET_BUFFER(in_pads[0], &apCh1In, &in_data_len1);
	DSP_GET_BUFFER(in_pads[1], &apCh2In, &in_data_len2);
	DSP_GET_BUFFER(&out_pads[0], (float**)&pTxBuf, &out_data_len);

	if (in_data_len1 != in_data_len2 )
	{
		CRITICAL_ERROR("bad input buffer size");
	}

	if  (sizeof(buffer_type_t) != 2 )
	{
		CRITICAL_ERROR("TODO");
	}

	while(in_data_len1--)
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

		out_val = (*apCh1In++) * FLOAT_NORMALIZER;
		*pTxBuf = (buffer_type_t)(out_val);
		pTxBuf++;

		out_val = (*apCh2In++) * FLOAT_NORMALIZER;
		*pTxBuf = (buffer_type_t)(out_val);
		pTxBuf++;


	}

	handle->max_out_val = max_out_val;


}





/**
 * I2S_mixer_ioctl()
 *
 * return:
 */
uint8_t I2S_mixer_ioctl(struct dsp_desc_t *adsp,
		uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2)
{
	struct I2S_MIXER_Instance_t *handle;

	handle = adsp->handle;

	switch(aIoctl_num)
	{
		case IOCTL_DSP_INIT :
			handle->enable_test_clipping = 0;
			handle->max_out_val = 0;
			break;

		case IOCTL_I2S_MIXER_ENABLE_TEST_CLIPPING :
			handle->enable_test_clipping = 1;
			break;

		case IOCTL_I2S_MIXER_DISABLE_TEST_CLIPPING :
			handle->enable_test_clipping = 0;
			break;

		case IOCTL_I2S_MIXER_GET_MAX_OUTPUT_VALUE :
			*(float*)aIoctl_param1 = handle->max_out_val  ;
			handle->max_out_val = 0;
			break;

		default :
			return 1;
	}
	return 0;
}



void  I2S_mixer_init(void)
{
	DSP_REGISTER_NEW_MODULE("I2S_mixer",
			I2S_mixer_ioctl , I2S_mixer_dsp, struct I2S_MIXER_Instance_t);
}

AUTO_INIT_FUNCTION(I2S_mixer_init);
