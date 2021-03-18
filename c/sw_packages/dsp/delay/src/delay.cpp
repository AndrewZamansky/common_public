/*
 *
 * file :   delay.c
 *
 *
 */
#include "_project_typedefs.h"
#include "_project_defines.h"

#include "dsp_management_api.h"
#include "dsp_management_internal_api.h"

#include "delay_api.h"
#include "delay.h"

#include "string.h"
#include "stdlib.h"
#include "auto_init_api.h"
extern "C" {
	#include "errors_api.h"
	#include "os_wrapper.h"
}

char delay_module_name[] = "delay";


static void memcpy_real(real_t *dest1, real_t *src1, size_t len)
{
	for( ; len ;len--)
	{
		*dest1++ = (*src1++);
	}
}

/**
 * delay_dsp()
 *
 * return:
 */
static void delay_dsp(struct dsp_module_inst_t *adsp)
{
	struct delay_instance_t *handle;
	real_t *apCh1In;
	real_t *apCh1Out;
	real_t *delay_buff;
	size_t data_len;
	size_t delay_samples;
	size_t samples_to_copy_from_delay_buffer;
	size_t samples_left_in_delay_buffer;
	uint8_t buff_is_zero_buffer;

	buff_is_zero_buffer = 1;
	dsp_get_input_buffer_from_pad(
			adsp, 0, (uint8_t**)&apCh1In, &data_len, &buff_is_zero_buffer);
	dsp_get_output_buffer_from_pad(adsp, 0, (uint8_t**)&apCh1Out, data_len);
	data_len = data_len / sizeof(real_t);

	handle = (struct delay_instance_t *)adsp->handle;
	delay_samples = handle->delay_samples;
	delay_buff = handle->buff;

	if (delay_samples <= data_len)
	{
		samples_to_copy_from_delay_buffer = delay_samples;
	}
	else
	{
		samples_to_copy_from_delay_buffer = data_len;
	}
	memcpy_real(apCh1Out, delay_buff, samples_to_copy_from_delay_buffer);
	memcpy_real(
			&apCh1Out[samples_to_copy_from_delay_buffer],
			apCh1In, data_len - samples_to_copy_from_delay_buffer);

	samples_left_in_delay_buffer =
			delay_samples - samples_to_copy_from_delay_buffer;
	//shift sample left in delay buffer
	memcpy_real(delay_buff,
			&delay_buff[samples_to_copy_from_delay_buffer],
			samples_left_in_delay_buffer);

	memcpy_real(&delay_buff[samples_left_in_delay_buffer],
			apCh1In, samples_to_copy_from_delay_buffer);
}



/**
 * delay_ioctl()
 *
 * return:
 */
static uint8_t delay_ioctl(struct dsp_module_inst_t *adsp,
		const uint8_t aIoctl_num, void * aIoctl_param1, void * aIoctl_param2)
{
	struct delay_instance_t *handle;
	uint16_t delay_samples;
	real_t *buff;

	handle = (struct delay_instance_t *)adsp->handle;
	switch(aIoctl_num)
	{
		case IOCTL_DSP_INIT :
			handle->delay_samples = 0;
			handle->buff =  NULL;
			break;
		case IOCTL_DELAY_SET_DELAY_SAMPLES :
			delay_samples = *((size_t*)aIoctl_param1);
			handle->delay_samples = delay_samples;
			buff = handle->buff;
			buff =
				(real_t *)os_safe_realloc(buff, delay_samples * sizeof(real_t));
			errors_api_check_if_malloc_succeed(buff);
			handle->buff = buff;
			memset(buff, 0, delay_samples * sizeof(real_t));
			break;
		default :
			return 1;
	}

	return 0;
}


/**
 * delay_init()
 *
 * return:
 */
extern "C" void  delay_init(void)
{
	DSP_REGISTER_NEW_MODULE("delay", delay_ioctl, delay_dsp,
			dsp_management_default_bypass, dsp_management_default_mute,
			0, struct delay_instance_t);
}

AUTO_INIT_FUNCTION(delay_init);
