/*
 *
 * file :   I2S_splitter.c
 *
 */



/********  includes *********************/
#include "_project_typedefs.h"
#include "_project_defines.h"

#include "dsp_management_api.h"
#include "dsp_management_internal_api.h"


#include "I2S_splitter_api.h"
#include "I2S_splitter.h"

#include "auto_init_api.h"

#include "_I2S_splitter_prerequirements_check.h"

/********  defines *********************/

#if (2 == NUM_OF_BYTES_PER_AUDIO_WORD)
	typedef int16_t	buffer_type_t	;
#elif (4 == NUM_OF_BYTES_PER_AUDIO_WORD)
	typedef int32_t	buffer_type_t	;
#else
	#error "TODO : "
#endif

/********  types  *********************/

/********  externals *********************/


/********  exported variables *********************/

char I2S_splitter_module_name[] = "I2S_splitter";


/**********   external variables    **************/



/***********   local variables    **************/

static real_t normalizer;



/**
 * I2S_splitter_dsp()
 *
 * return:
 */
void I2S_splitter_dsp(struct dsp_module_inst_t *adsp,
		struct dsp_pad_t *in_pads[MAX_NUM_OF_OUTPUT_PADS] ,
		struct dsp_pad_t out_pads[MAX_NUM_OF_OUTPUT_PADS])
{
	real_t *apChOut[MAX_NUM_OF_OUTPUT_PADS];
	size_t in_data_len;
	size_t out_data_len[MAX_NUM_OF_OUTPUT_PADS] ;
	buffer_type_t *pRxBuf;
	real_t in_real;
	uint8_t num_of_channels ;
	struct I2S_splitter_instance_t *handle;
	uint8_t i;


	handle = (struct I2S_splitter_instance_t *)adsp->handle;
	num_of_channels = handle->num_of_channels;

	if (num_of_channels > MAX_NUM_OF_OUTPUT_PADS )
	{
		CRITICAL_ERROR("to many channels");
	}

	/*
	 * casting here is just to avoid warning as we are aware that
	 * pRxBuf has some INT type
	 */
	dsp_get_buffer_from_pad(in_pads[0], (real_t**)&pRxBuf, &in_data_len);


	for (i = 0; i < num_of_channels; i++)
	{
		dsp_get_buffer_from_pad(&out_pads[i], &apChOut[i], &out_data_len[i]);
	}


	if (out_data_len[0] != out_data_len[1] )
	{
		CRITICAL_ERROR("bad output buffer size");
	}

	in_data_len /= ( num_of_channels * NUM_OF_BYTES_PER_AUDIO_WORD);

	if (in_data_len != out_data_len[0] )
	{
		CRITICAL_ERROR("bad input buffer size");
	}

	while (in_data_len--)
	{
		for (i = 0; i < num_of_channels; i++)
		{
			real_t *curr_ChOut;

			curr_ChOut = apChOut[i];
			in_real = *pRxBuf++;
			*curr_ChOut++ = in_real * normalizer;
			apChOut[i] = curr_ChOut;
		}
	}

}





/**
 * I2S_splitter_ioctl()
 *
 * return:
 */
uint8_t I2S_splitter_ioctl(struct dsp_module_inst_t *adsp ,
		uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2)
{
	struct I2S_splitter_instance_t *handle;

	handle = (struct I2S_splitter_instance_t *)adsp->handle;

	switch(aIoctl_num)
	{
	case IOCTL_DSP_INIT :
		handle->num_of_channels = 2;
		break;
	case IOCTL_I2S_SPLITTER_SET_NUM_OF_CHANNELS :
		handle->num_of_channels = *(uint8_t*)aIoctl_param1;
		break;

	default :
		return 1;
	}
	return 0;
}




void  I2S_splitter_init(void)
{
#if defined(CONFIG_DSP_REAL_NUMBER_FORMAT_FLOATING_POINT)
	#if (2 == NUM_OF_BYTES_PER_AUDIO_WORD)
		normalizer = (float)(1.0f / (float)0x7fff);
	#elif (4 == NUM_OF_BYTES_PER_AUDIO_WORD)
		normalizer = (float)(1.0f / (float)0x7fffffff);
	#else
		#error "TODO : "
	#endif
#elif defined(CONFIG_DSP_REAL_NUMBER_FORMAT_FIXED_POINT)
	#if (2 == NUM_OF_BYTES_PER_AUDIO_WORD)
		normalizer = (float)(1.0f / (float)0x7fff);
	#else
		#error "TODO : for audio word with 4 bytes devision by  0x7fffffff is wrong because integer part is 16 bit only "
	#endif
#else
	#error "undefined real numbers format"
#endif

	DSP_REGISTER_NEW_MODULE("I2S_splitter",	I2S_splitter_ioctl,
					I2S_splitter_dsp , struct I2S_splitter_instance_t);
}

AUTO_INIT_FUNCTION(I2S_splitter_init);
