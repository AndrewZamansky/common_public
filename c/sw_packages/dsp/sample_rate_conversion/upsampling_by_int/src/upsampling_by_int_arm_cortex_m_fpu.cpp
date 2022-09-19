/*
 * file :   upsampling_by_int_arm_cortex_m.c
 *
 */
#include "_project_typedefs.h"
#include "_project_defines.h"

#include "dsp_management_api.h"
#include "common_dsp_api.h"

#include "cpu_config.h"
#include "arm_math.h"
#include "upsampling_by_int.h"
#include "upsampling_by_int_api.h"
extern "C" {
	#include "errors_api.h"
	#include "os_wrapper.h"
}


struct upsampling_by_int_t {
	void *p_filter_instance;
	float *p_filter_state;
	real_t *p_coefficients;
	uint32_t expected_number_of_input_samples;
};


void downsampling_by_int_function(void *p_filter,
	real_t *in_buf, real_t *tmp_buf, size_t in_buff_len, real_t *out_buf)
{
	arm_fir_interpolate_instance_f32 *filter_params;
	struct upsampling_by_int_t * upsampling_by_int;

	upsampling_by_int = (struct upsampling_by_int_t *)upsampling_by_int;
	filter_params = (arm_fir_interpolate_instance_f32*)(
									upsampling_by_int->p_filter_instance);
	in_buff_len = in_buff_len / sizeof(float);
	if (in_buff_len != upsampling_by_int->expected_number_of_input_samples)
	{
		CRITICAL_ERROR("input data size does not match init data size");
	}
	arm_fir_interpolate_f32(filter_params, in_buf, out_buf, in_buff_len);
}


/*  func : firs_free()
 */
void upsampling_by_int_free(void *pFilter)
{
	struct upsampling_by_int_t *p_filter;

	p_filter = (struct upsampling_by_int_t *)pFilter;
	os_safe_free(p_filter->p_filter_state);
	os_safe_free(p_filter->p_filter_instance);
	os_safe_free(p_filter->p_coefficients);
	os_safe_free(pFilter);
}


/*   func : upsampling_by_int_alloc()
 */
void upsampling_by_int_create(
		struct upsampling_by_int_instance_t *handle,
		struct fir_filter_api_set_params_t *fir_set_params)
{
	arm_status	status;
	size_t expected_number_of_input_samples;
	size_t number_of_filter_coefficients;
	struct upsampling_by_int_t *p_upsampling_by_int;
	arm_fir_interpolate_instance_f32* p_filter_instance;
	float *p_filter_state;
	uint32_t factor;
	real_t *p_coefficients;
	struct fir_filter_api_set_coefficients_by_params_t  *coeff_by_params;

	p_upsampling_by_int = (struct upsampling_by_int_t *)malloc(
									sizeof(struct upsampling_by_int_t));
	errors_api_check_if_malloc_succeed(p_upsampling_by_int);

	p_filter_instance = (arm_fir_interpolate_instance_f32*)malloc(
							sizeof( arm_fir_interpolate_instance_f32));
	errors_api_check_if_malloc_succeed(p_filter_instance);
	p_upsampling_by_int->p_filter_instance = p_filter_instance;

	expected_number_of_input_samples =
			fir_set_params->expected_number_of_input_samples;
	factor = handle->factor;
	number_of_filter_coefficients =
			fir_set_params->number_of_filter_coefficients;
	p_filter_state = (float*)malloc(sizeof(float) *
			((number_of_filter_coefficients / factor) +
					expected_number_of_input_samples - 1));
	errors_api_check_if_malloc_succeed(p_filter_state);
	p_upsampling_by_int->p_filter_state = p_filter_state;

	p_coefficients = (real_t *)malloc(
			number_of_filter_coefficients * sizeof(real_t));
	errors_api_check_if_malloc_succeed(p_coefficients);
	p_upsampling_by_int->p_coefficients = p_coefficients;

	coeff_by_params = &fir_set_params->coeff_by_params;
	fir_coefficients_calculation(coeff_by_params->filter_mode,
			coeff_by_params->fc, coeff_by_params->A_stop,
			coeff_by_params->dfc, coeff_by_params->sample_rate_Hz,
			p_coefficients, number_of_filter_coefficients);

	if (0xff < handle->factor)
	{
		CRITICAL_ERROR('downsampling factor is to big');
	}
	status = arm_fir_interpolate_init_f32 (p_filter_instance ,
			(uint8_t)factor, (uint16_t)number_of_filter_coefficients ,
			p_coefficients, p_filter_state, expected_number_of_input_samples);

	if (ARM_MATH_SUCCESS != status)
	{
		upsampling_by_int_free(p_upsampling_by_int);
		p_upsampling_by_int = NULL;
	}
	p_upsampling_by_int->expected_number_of_input_samples =
								expected_number_of_input_samples;
	handle->p_upsampling_by_int_filter = p_upsampling_by_int;
}
