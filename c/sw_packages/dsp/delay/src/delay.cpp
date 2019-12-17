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
void delay_dsp(struct dsp_module_inst_t *adsp)
{
	struct delay_instance_t *handle;
	real_t *apCh1In;
	real_t *apCh1Out;
	real_t *delay_buff;
	size_t in_data_len1 ;
	size_t out_data_len1 ;
	uint16_t delay_samples;

	handle = (struct delay_instance_t *)adsp->handle;

	dsp_get_input_buffer_from_pad(adsp, 0, &apCh1In, &in_data_len1);
	dsp_get_output_buffer_from_pad(adsp, 0, &apCh1Out, &out_data_len1);

	if (in_data_len1 != out_data_len1 )
	{
		CRITICAL_ERROR("bad buffers sizes");
	}

	delay_samples = handle->delay_samples;
	delay_buff = handle->buff;

	memcpy_real(apCh1Out, delay_buff, delay_samples);
	memcpy_real(
			&apCh1Out[delay_samples], apCh1In, in_data_len1 - delay_samples);
	memcpy_real(delay_buff, apCh1In, delay_samples);
}



/**
 * delay_ioctl()
 *
 * return:
 */
uint8_t delay_ioctl(struct dsp_module_inst_t *adsp,
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
			delay_samples = *((uint16_t*)aIoctl_param1);
			handle->delay_samples = delay_samples;
			buff = handle->buff;
			buff = (real_t *)realloc(buff, delay_samples * sizeof(real_t));
			if (NULL == buff)
			{
				CRITICAL_ERROR("not enough memory in heap");
			}
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
	DSP_REGISTER_NEW_MODULE("delay",
			delay_ioctl, delay_dsp, struct delay_instance_t);
}

AUTO_INIT_FUNCTION(delay_init);
