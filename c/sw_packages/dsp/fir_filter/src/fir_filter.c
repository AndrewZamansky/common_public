/*
 *
 * file :   fir_filter.c
 *
 *
 */
#include "_project_typedefs.h"
#include "_project_defines.h"

#include "dsp_management_api.h"
#include "common_dsp_api.h"


#include "fir_filter_api.h"
#include "fir_filter.h"

#include "auto_init_api.h"
#include "math.h"


char fir_filter_module_name[] = "fir_filter";

/**
 * fir_filter_dsp()
 *
 * return:
 */
static void fir_filter_dsp(struct dsp_module_inst_t *adsp)
{
	float *apCh1In  ;
	float *apCh1Out ;
	size_t data_len ;
	struct FIR_FILTER_Instance_t *handle;
	uint8_t buff_is_zero_buffer;

	buff_is_zero_buffer = 1;
	dsp_get_input_buffer_from_pad(
			adsp, 0, &(uint8_t*)apCh1In, &data_len, &buff_is_zero_buffer);
	dsp_get_output_buffer_from_pad(adsp, 0, &(uint8_t*)apCh1Out, data_len);

	handle = adsp->handle;
	if(0 == handle->number_of_filter_coefficients) return;

	fir_filter_function(handle->p_fir_filter,
			apCh1In, apCh1Out, in_data_len / sizeof(float));
}


/**
 * fir_filter_ioctl()
 *
 * return:
 */
uint8_t fir_filter_ioctl(struct dsp_module_inst_t *adsp,
		const uint8_t aIoctl_num, void * aIoctl_param1, void * aIoctl_param2)
{
	size_t number_of_filter_coefficients;
	size_t predefined_data_block_size;
	float *p_coefficients;
	struct fir_filter_api_set_params_t *p_band_set_params;
	struct FIR_FILTER_Instance_t *handle;

	handle = adsp->handle;
	switch(aIoctl_num)
	{
	case IOCTL_DSP_INIT :
		handle->number_of_filter_coefficients =0;
		handle->p_coefficients = NULL ;

		break;


	case IOCTL_FIR_FILTER_SET_FIR_COEFFICIENTS :
		p_band_set_params =
				((struct fir_filter_api_set_params_t *)aIoctl_param1);
		number_of_filter_coefficients =
				p_band_set_params->number_of_filter_coefficients;
		handle->number_of_filter_coefficients = number_of_filter_coefficients;
		predefined_data_block_size =
				p_band_set_params->predefined_data_block_size;
		handle->predefined_data_block_size = predefined_data_block_size;

		p_coefficients=(float *)realloc(handle->p_coefficients,
				sizeof(float) * number_of_filter_coefficients);
		errors_api_check_if_malloc_succeed(p_coefficients);

		memcpy(p_coefficients, p_band_set_params->p_coefficients,
							sizeof(float) * number_of_filter_coefficients);
		handle->p_coefficients = p_coefficients;

		handle->p_fir_filter = fir_alloc( number_of_filter_coefficients,
								p_coefficients , predefined_data_block_size );
		break;

	default :
		return 1;
	}
	return 0;
}


/**
 * fir_filter_init()
 *
 * return:
 */
extern "C" void  fir_filter_init(void)
{
	DSP_REGISTER_NEW_MODULE("fir_filter", fir_filter_ioctl, fir_filter_dsp,
			dsp_management_default_bypass, dsp_management_default_mute,
			0, struct FIR_FILTER_Instance_t);
}

AUTO_INIT_FUNCTION(fir_filter_init);
