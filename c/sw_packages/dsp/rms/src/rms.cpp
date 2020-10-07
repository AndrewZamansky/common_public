/*
 *
 * file :   rms.c
 *
 *
 */
#include "_project_typedefs.h"
#include "_project_defines.h"

#include "dsp_management_api.h"
#include "dsp_management_internal_api.h"

#include "rms_api.h"
#include "rms.h"

#include "string.h"
#include "stdlib.h"

#include "auto_init_api.h"


char rms_module_name[] = "rms";


#define  ALPHA   0.00004f
#define  ONE_MINUS_ALPHA   (1.0f - ALPHA)

/**
 * rms_dsp()
 *
 * return:
 */
static void rms_dsp(struct dsp_module_inst_t *adsp)
{
	struct rms_instance_t *handle;
	real_t *apCh1In;
	real_t *apCh1Out;
	size_t data_len;
	real_t prev_rms_sq;
	real_t curr_x1;
	uint8_t buff_is_zero_buffer;

	buff_is_zero_buffer = 1;
	dsp_get_input_buffer_from_pad(
			adsp, 0, &(uint8_t*)apCh1In, &data_len, &buff_is_zero_buffer);
	dsp_get_output_buffer_from_pad(adsp, 0, &(uint8_t*)apCh1Out, data_len);

	handle = (struct rms_instance_t *)adsp->handle;
	prev_rms_sq = handle->prev_rms_sq;

	data_len = data_len / sizeof(real_t);
	while (data_len--)
	{
		curr_x1 = *apCh1In++;
		curr_x1 *= curr_x1;
		curr_x1 *=  ALPHA;
		prev_rms_sq *= ONE_MINUS_ALPHA;
		prev_rms_sq +=curr_x1;

		*apCh1Out++ = prev_rms_sq;
	}
	handle->prev_rms_sq = prev_rms_sq;
}



/**
 * rms_ioctl()
 *
 * return:
 */
static uint8_t rms_ioctl(struct dsp_module_inst_t *adsp,
		const uint8_t aIoctl_num, void * aIoctl_param1, void * aIoctl_param2)
{
	struct rms_instance_t *handle;

	handle = (struct rms_instance_t *)adsp->handle;
	switch(aIoctl_num)
	{
		case IOCTL_DSP_INIT :
			handle->prev_rms_sq = 0;
			break;
		default :
			return 1;
	}

	return 0;
}


/**
 * rms_init()
 *
 * return:
 */
extern "C" void  rms_init(void)
{
	DSP_REGISTER_NEW_MODULE("rms", rms_ioctl, rms_dsp,
			dsp_management_default_bypass, dsp_management_default_mute,
			0, struct rms_instance_t);
}

AUTO_INIT_FUNCTION(rms_init);
