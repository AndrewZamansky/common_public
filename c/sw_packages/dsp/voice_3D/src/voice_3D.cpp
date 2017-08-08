/*
 *
 * file :   voice_3D.c
 *
 *
 */



/********  includes *********************/
#include "_project_typedefs.h"
#include "_project_defines.h"

#include "dsp_management_api.h"
#include "dsp_management_internal_api.h"

#include "voice_3D_api.h"
#include "voice_3D.h"

#include "math.h"

#include "auto_init_api.h"


#include "_voice_3D_prerequirements_check.h"

/********  defines *********************/


/********  types  *********************/

/********  externals *********************/


/********  exported variables *********************/

char voice_3D_module_name[] = "voice_3D";


/**********   external variables    **************/



/***********   local variables    **************/


/**
 * voice_3D_dsp()
 *
 * return:
 */
void voice_3D_dsp(struct dsp_desc_t *adsp,
		struct dsp_pad_t *in_pads[MAX_NUM_OF_OUTPUT_PADS],
		struct dsp_pad_t out_pads[MAX_NUM_OF_OUTPUT_PADS])
{
	struct VOICE_3D_Instance_t *handle;
	real_t *apCh1In;
	real_t *apCh2In;
	real_t *apCh1Out;
	real_t *apCh2Out;
	size_t in_data_len1 ;
	size_t in_data_len2 ;
	size_t out_data_len1 ;
	size_t out_data_len2 ;

	real_t main_ch_gain;
	real_t second_ch_gain;

	real_t medium_gain;
	real_t side_gain;
	real_t _3D_gain;

	handle = (struct VOICE_3D_Instance_t *)adsp->handle;

	DSP_GET_BUFFER(in_pads[0], &apCh1In, &in_data_len1);
	DSP_GET_BUFFER(in_pads[1], &apCh2In, &in_data_len2);
	DSP_GET_BUFFER(&out_pads[0], &apCh1Out, &out_data_len1);
	DSP_GET_BUFFER(&out_pads[1], &apCh2Out, &out_data_len2);

	if (in_data_len1 != in_data_len2 )
	{
		CRITICAL_ERROR("bad input buffer size");
	}

	if (out_data_len1 != out_data_len2 )
	{
		CRITICAL_ERROR("bad output buffer size");
	}

	if (in_data_len1 > out_data_len1 )
	{
		CRITICAL_ERROR("bad buffers sizes");
	}


	medium_gain = handle->medium_gain ;
	side_gain = handle->side_gain ;
	_3D_gain = handle->_3D_gain ;

	main_ch_gain = medium_gain + side_gain ;
	second_ch_gain = medium_gain + side_gain + _3D_gain ;

	while(in_data_len1--)
	{
		real_t curr_ch_1;
		real_t curr_ch_2;
		real_t tmp;
		real_t tmp1;

		curr_ch_1 = *apCh1In++;
		curr_ch_2 = *apCh2In++;

		tmp = main_ch_gain * curr_ch_1;
		tmp1 = curr_ch_2 * second_ch_gain;
		tmp +=tmp1;
		*apCh1Out++ = tmp;

		tmp = main_ch_gain * curr_ch_2;
		tmp1 = curr_ch_1 * second_ch_gain;
		tmp +=tmp1;
		*apCh2Out++ = tmp;
	}

}


/**
 * voice_3D_ioctl()
 *
 * return:
 */
uint8_t voice_3D_ioctl(struct dsp_desc_t *adsp,
		const uint8_t aIoctl_num, void * aIoctl_param1, void * aIoctl_param2)
{
	struct VOICE_3D_Instance_t *handle;

	handle = (struct VOICE_3D_Instance_t *)adsp->handle;
	switch(aIoctl_num)
	{
		case IOCTL_DSP_INIT :
			handle->medium_gain = (float)0.5;
			handle->side_gain =  (float)0.5;
			handle->_3D_gain = (float)0;
			break;
		case IOCTL_VOICE_3D_SET_MEDIUM_GAIN :
			handle->medium_gain = (*((float*)aIoctl_param1)) / (float)2;
			break;
		case IOCTL_VOICE_3D_SET_SIDE_GAIN :
			handle->side_gain = (*((float*)aIoctl_param1)) / (float)2;
			break;
		case IOCTL_VOICE_3D_SET_3D_GAIN :
			handle->_3D_gain = *((float*)aIoctl_param1);
			break;
		default :
			return 1;
	}
	return 0;
}




/**
 * voice_3D_init()
 *
 * return:
 */
void  voice_3D_init(void)
{
	DSP_REGISTER_NEW_MODULE("voice_3D",
			voice_3D_ioctl, voice_3D_dsp, struct VOICE_3D_Instance_t);
}

AUTO_INIT_FUNCTION(voice_3D_init);
