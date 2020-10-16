/*
 *
 * file :   downsampling_by_int_filter.c
 *
 *
 *
 *
 *
 */


#include "_project_typedefs.h"
#include "_project_defines.h"

#include "dsp_management_api.h"
#include "common_dsp_api.h"

#include "downsampling_by_int_api.h"
#include "downsampling_by_int.h"

#include "auto_init_api.h"
#include "math.h"


char downsampling_by_int_module_name[] = "downsampling_by_int";


/**
 * downsampling_by_int_dsp()
 *
 * return:
 */
static void downsampling_by_int_dsp(struct dsp_module_inst_t *adsp)
{
	float *apCh1In  ;
	float *apCh1Out ;
	struct DOWNSAMPLING_BY_INT_Instance_t *handle;
	size_t data_len ;
	void *p_downsampling_by_int_filter ;
	size_t factor ;
	uint8_t buff_is_zero_buffer;

	handle = adsp->handle;
	p_downsampling_by_int_filter = handle->p_downsampling_by_int_filter;
	factor = handle->factor;
	if((0 == handle->number_of_filter_coefficients) || (0 == factor) ||
		(NULL == p_downsampling_by_int_filter))
	{
		return;
	}

	buff_is_zero_buffer = 1;
	dsp_get_input_buffer_from_pad(
			adsp, 0, &(uint8_t*)apCh1In, &data_len, &buff_is_zero_buffer);
	dsp_get_output_buffer_from_pad(
			adsp, 0, &(uint8_t*)apCh1Out, data_len / factor);

	downsampling_by_int_function(
			p_downsampling_by_int_filter, apCh1In, apCh1Out, data_len);
}


/**
 * downsampling_by_int_ioctl()
 *
 * return:
 */
static uint8_t downsampling_by_int_ioctl(struct dsp_module_inst_t *adsp,
		const uint8_t aIoctl_num, void * aIoctl_param1, void * aIoctl_param2)
{
	size_t number_of_filter_coefficients;
	size_t predefined_data_block_size;
	float *p_coefficients;
	struct downsampling_by_int_api_set_params_t *p_band_set_params;
	struct DOWNSAMPLING_BY_INT_Instance_t *handle;

	handle = adsp->handle;
	switch(aIoctl_num)
	{
	case IOCTL_DSP_INIT :
		handle->factor =0;
		handle->number_of_filter_coefficients =0;
		handle->p_coefficients = NULL ;
		handle->p_downsampling_by_int_filter = NULL;

		break;

	case IOCTL_DOWNSAMPLING_BY_INT_SET_FACTOR :
		handle->factor = *(size_t*)aIoctl_param1;

		break;

	case IOCTL_DOWNSAMPLING_BY_INT_SET_FIR_COEFFICIENTS :
		p_band_set_params =
				((struct downsampling_by_int_api_set_params_t *)aIoctl_param1);
		number_of_filter_coefficients =
				p_band_set_params->number_of_filter_coefficients;
		handle->number_of_filter_coefficients = number_of_filter_coefficients;
		predefined_data_block_size =
				p_band_set_params->predefined_data_block_size;
		handle->predefined_data_block_size = predefined_data_block_size;

		p_coefficients=(float *)os_safe_realloc(handle->p_coefficients,
						sizeof(float) * number_of_filter_coefficients);
		errors_api_check_if_malloc_succeed(p_coefficients);

		memcpy(p_coefficients,p_band_set_params->p_coefficients,
							sizeof(float) * number_of_filter_coefficients);
		handle->p_coefficients = p_coefficients;

		handle->p_downsampling_by_int_filter =
				downsampling_by_int_alloc(handle->factor,
						number_of_filter_coefficients,
						p_coefficients, predefined_data_block_size );
		break;

	default :
		return 1;
	}
	return 0;
}


/**
 * downsampling_by_int_init()
 *
 * return:
 */
extern "C" void  downsampling_by_int_init(void)
{
	DSP_REGISTER_NEW_MODULE("downsampling_by_int", downsampling_by_int_ioctl,
			downsampling_by_int_dsp,
			NULL, dsp_management_default_mute,
			DSP_MANAGEMENT_FLAG_BYPASS_NOT_AVAILABLE,
			struct DOWNSAMPLING_BY_INT_Instance_t);
}

AUTO_INIT_FUNCTION(downsampling_by_int_init);
