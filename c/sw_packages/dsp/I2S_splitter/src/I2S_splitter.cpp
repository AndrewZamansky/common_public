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
	#define	FLOAT_NORMALIZER	0x7fff
	typedef int16_t	buffer_type_t	;
#endif
#if (4 == NUM_OF_BYTES_PER_AUDIO_WORD)
	#error "TODO"
	#define	FLOAT_NORMALIZER	0x7fffffff
	typedef int32_t	buffer_type_t	;
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
void I2S_splitter_dsp(struct dsp_desc_t *adsp,
		struct dsp_pad_t *in_pads[MAX_NUM_OF_OUTPUT_PADS] ,
		struct dsp_pad_t out_pads[MAX_NUM_OF_OUTPUT_PADS])
{
	real_t *apCh1Out;

	real_t *apCh2Out;
	size_t in_data_len;
	size_t out_data_len1 ;
	size_t out_data_len2 ;
	buffer_type_t *pRxBuf;
	real_t in_real;

	DSP_GET_BUFFER(in_pads[0], (real_t**)&pRxBuf, &in_data_len);
	DSP_GET_BUFFER(&out_pads[0], &apCh1Out, &out_data_len1);
	DSP_GET_BUFFER(&out_pads[1], &apCh2Out, &out_data_len2);

	if (out_data_len1 != out_data_len2 )
	{
		CRITICAL_ERROR("bad output buffer size");
	}


	in_data_len /= ( 2 * NUM_OF_BYTES_PER_AUDIO_WORD); // 2 ch

	if (in_data_len != out_data_len1 )
	{
		CRITICAL_ERROR("bad input buffer size");
	}

	while (in_data_len--)
	{
		in_real = *pRxBuf++;
		*apCh1Out++ = in_real * normalizer;
		in_real = *pRxBuf++;
		*apCh2Out++ = in_real * normalizer;
	}

}





/**
 * I2S_splitter_ioctl()
 *
 * return:
 */
uint8_t I2S_splitter_ioctl(struct dsp_desc_t *adsp ,
		uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2)
{
	switch(aIoctl_num)
	{
		case IOCTL_DSP_INIT :


			break;

		default :
			return 1;
	}
	return 0;
}




void  I2S_splitter_init(void)
{
#if (2 == NUM_OF_BYTES_PER_AUDIO_WORD)
	normalizer = (float)(1.0f / (float)0x7fff);
#else
	#error "TODO : for audio word with 4 bytes devision by  0x7fffffff is wrong because integer part is 16 bit only "
#endif
	DSP_REGISTER_NEW_MODULE("I2S_splitter",	I2S_splitter_ioctl,
					I2S_splitter_dsp , struct I2S_SPLITTER_Instance_t);
}

AUTO_INIT_FUNCTION(I2S_splitter_init);
