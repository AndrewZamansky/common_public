/*
 *
 * file :   fir_filter.c
 *
 *
 */
#include "_project_typedefs.h"
#include "_project_defines.h"

extern "C" {
#include "errors_api.h"
#include "os_wrapper.h"
}

#include "dsp_management_api.h"
#include "common_dsp_api.h"
#include "dsp_management_internal_api.h"
#include "fir_filter_api.h"
#include "fir_filter.h"

#include "auto_init_api.h"
#include "math.h"

extern void fir_coefficients_calculation( enum fir_filter_mode_e filter_mode,
		real_t fc, real_t Astop, real_t dfc,
		real_t sample_rate, real_t *p_coeffs, size_t coeff_num);
char fir_filter_module_name[] = "fir_filter";

/**
 * fir_filter_dsp()
 *
 * return:
 */
static void fir_filter_dsp(struct dsp_module_inst_t *adsp)
{
	real_t *apCh1In  ;
	real_t *apCh1Out ;
	size_t data_len ;
	struct fir_filter_t *handle;
	uint8_t buff_is_zero_buffer;

	buff_is_zero_buffer = 1;
	dsp_get_input_buffer_from_pad(
			adsp, 0, &(uint8_t*)apCh1In, &data_len, &buff_is_zero_buffer);
	dsp_get_output_buffer_from_pad(adsp, 0, &(uint8_t*)apCh1Out, data_len);

	handle = (struct fir_filter_t *)adsp->handle;
	if (NULL == handle->p_fir_filter) return;

	fir_filter_function(handle->p_fir_filter,
			apCh1In, apCh1Out, data_len / sizeof(float));
}


static uint8_t create_filter(struct fir_filter_t *handle,
		struct fir_filter_api_set_params_t *p_set_params)
{
	struct fir_filter_api_set_coefficients_by_params_t *fir_params;
	size_t number_of_filter_coefficients;
	size_t predefined_data_block_size;
	real_t *p_coefficients;
	float *p_new_coefficients;
	size_t i;
	real_t sample_rate;

	if (NULL != handle->p_fir_filter)
	{
		fir_free(handle->p_fir_filter);
		handle->p_fir_filter = NULL;
		handle->number_of_filter_coefficients = 0;
	}
	number_of_filter_coefficients = p_set_params->number_of_filter_coefficients;
	handle->number_of_filter_coefficients = number_of_filter_coefficients;
	predefined_data_block_size = p_set_params->predefined_data_block_size;
	handle->predefined_data_block_size = predefined_data_block_size;

	p_coefficients = (real_t *)os_safe_realloc(handle->p_coefficients,
							sizeof(real_t) * number_of_filter_coefficients);
	errors_api_check_if_malloc_succeed(p_coefficients);
	handle->p_coefficients = p_coefficients;

	switch (p_set_params->set_coefficients_type)
	{
	case FIR_SET_RAW_COEFFICIENTS:
		p_new_coefficients = p_set_params->raw_coefficients.p_coefficients;
		for (i = 0; i < number_of_filter_coefficients; i++)
		{
			p_coefficients[i] = p_new_coefficients[i]; // this will cast also
		}
		break;
	case FIR_CALCULATE_COEFFICIENTS_BY_PARAMS:
		fir_params = &p_set_params->coeff_by_params;
		sample_rate = (real_t)fir_params->sample_rate_Hz;
		fir_coefficients_calculation(fir_params->filter_mode, fir_params->fc,
				fir_params->A_stop, fir_params->dfc, sample_rate,
				p_coefficients, number_of_filter_coefficients);
		break;
	default:
		CRITICAL_ERROR("unsupported type of fir coefficients");
		break;
	}

	handle->p_fir_filter = fir_alloc( number_of_filter_coefficients,
							p_coefficients, predefined_data_block_size);
	return 0;
}


/**
 * fir_filter_ioctl()
 *
 * return:
 */
static uint8_t fir_filter_ioctl(struct dsp_module_inst_t *adsp,
		const uint8_t aIoctl_num, void * aIoctl_param1, void * aIoctl_param2)
{
	struct fir_filter_t *handle;

	handle = (struct fir_filter_t *)adsp->handle;
	switch(aIoctl_num)
	{
	case IOCTL_DSP_INIT :
		handle->p_fir_filter = NULL;
		handle->number_of_filter_coefficients = 0;
		handle->predefined_data_block_size = 0;
		handle->p_coefficients = NULL;
		break;
	case IOCTL_FIR_FILTER_SET_FIR_COEFFICIENTS :
		return create_filter(handle,
				(struct fir_filter_api_set_params_t *)aIoctl_param1);
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
			0, struct fir_filter_t);
}

AUTO_INIT_FUNCTION(fir_filter_init);
