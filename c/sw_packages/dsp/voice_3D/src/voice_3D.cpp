/*
 *
 * file :   voice_3D.c
 *
 *
 */

#include "_project_typedefs.h"
#include "_project_defines.h"

#include "dsp_management_api.h"
#include "dsp_management_internal_api.h"

#include "voice_3D_api.h"
#include "voice_3D.h"

#include "math.h"

#include "auto_init_api.h"


char voice_3D_module_name[] = "voice_3D";

/**
 * voice_3D_dsp()
 *
 * return:
 */
static void voice_3D_dsp(struct dsp_module_inst_t *adsp)
{
	struct VOICE_3D_Instance_t *handle;
	real_t *apCh1In;
	real_t *apCh2In;
	real_t *apCh1Out;
	real_t *apCh2Out;
	size_t data_len;
	uint8_t buff_is_zero_buffer;
	real_t C;
	real_t D;

	buff_is_zero_buffer = 1;
	dsp_get_input_buffer_from_pad(
			adsp, 0, &(uint8_t*)apCh1In, &data_len, &buff_is_zero_buffer);
	dsp_get_input_buffer_from_pad(
			adsp, 1, &(uint8_t*)apCh2In, &data_len, &buff_is_zero_buffer);
	dsp_get_output_buffer_from_pad(adsp, 0, &(uint8_t*)apCh1Out, data_len);
	dsp_get_output_buffer_from_pad(adsp, 1, &(uint8_t*)apCh2Out, data_len);

	handle = (struct VOICE_3D_Instance_t *)adsp->handle;
	C = handle->C;
	D = handle->D;

	data_len = data_len / sizeof(real_t);
	while (data_len--)
	{
		real_t curr_ch_1;
		real_t curr_ch_2;
		real_t tmp;
		real_t tmp1;

		curr_ch_1 = *apCh1In++;
		curr_ch_2 = *apCh2In++;

		tmp = C * curr_ch_1;
		tmp1 = D * curr_ch_2;
		tmp +=tmp1;
		*apCh1Out++ = tmp;

		tmp = D * curr_ch_1;
		tmp1 = C * curr_ch_2;
		tmp +=tmp1;
		*apCh2Out++ = tmp;
	}
}

static void update_coefficients(struct VOICE_3D_Instance_t * handle)
{
	real_t center_gain;
	real_t side_gain;
	real_t _3D_gain;
	real_t one;
	real_t zero;
	real_t A;
	real_t B;
	real_t C;
	real_t D;

	one = (int16_t)1;
	zero = (int16_t)0;

	center_gain = handle->center_gain;
	side_gain = handle->side_gain;
	_3D_gain = handle->_3D_gain;

	if ( zero == handle->enable_voice)
	{
		center_gain = one;
		side_gain = one;
	}

	if ( zero == handle->enable_3D)
	{
		_3D_gain = zero;
	}

	A = one - (_3D_gain * 0.2f);
	B = one + (_3D_gain * 1.45f);

	C = (center_gain * A) + (side_gain * B);
	D = (center_gain * A) - (side_gain * B);

	C = C * 0.5f;
	D = D * 0.5f;

	handle->C = C;
	handle->D = D;
}


/**
 * voice_3D_ioctl()
 *
 * return:
 */
static uint8_t voice_3D_ioctl(struct dsp_module_inst_t *adsp,
		const uint8_t aIoctl_num, void * aIoctl_param1, void * aIoctl_param2)
{
	struct VOICE_3D_Instance_t *handle;

	handle = (struct VOICE_3D_Instance_t *)adsp->handle;
	switch(aIoctl_num)
	{
		case IOCTL_DSP_INIT :
			handle->center_gain = (float)1;
			handle->side_gain =  (float)1;
			handle->_3D_gain = (float)0;
			handle->enable_voice = (float)1;
			handle->enable_3D = (float)1;
			break;
		case IOCTL_VOICE_3D_SET_CENTER_GAIN :
			handle->center_gain = (*((float*)aIoctl_param1));
			break;
		case IOCTL_VOICE_3D_SET_SIDE_GAIN :
			handle->side_gain = (*((float*)aIoctl_param1));
			break;
		case IOCTL_VOICE_3D_SET_3D_GAIN :
			handle->_3D_gain = *((float*)aIoctl_param1);
			break;
		case IOCTL_VOICE_3D_SET_VOICE_ON :
			handle->enable_voice = *((uint8_t*)aIoctl_param1);
			break;
		case IOCTL_VOICE_3D_SET_3D_ON :
			handle->enable_3D = *((uint8_t*)aIoctl_param1);
			break;
		case IOCTL_VOICE_3D_GET_CENTER_GAIN :
			*((float*)aIoctl_param1) = handle->center_gain;
			break;
		case IOCTL_VOICE_3D_GET_SIDE_GAIN :
			*((float*)aIoctl_param1) = handle->side_gain;
			break;
		case IOCTL_VOICE_3D_GET_3D_GAIN :
			*((float*)aIoctl_param1) = handle->_3D_gain;
			break;
		case IOCTL_VOICE_3D_GET_VOICE_ON :
			*((uint8_t*)aIoctl_param1) = handle->enable_voice;
			break;
		case IOCTL_VOICE_3D_GET_3D_ON :
			*((uint8_t*)aIoctl_param1) = handle->enable_3D;
			break;
		default :
			return 1;
	}

	update_coefficients(handle);

	return 0;
}


/**
 * voice_3D_init()
 *
 * return:
 */
extern "C" void  voice_3D_init(void)
{
	DSP_REGISTER_NEW_MODULE("voice_3D", voice_3D_ioctl, voice_3D_dsp,
			dsp_management_default_bypass, dsp_management_default_mute,
			0, struct VOICE_3D_Instance_t);
}

AUTO_INIT_FUNCTION(voice_3D_init);
