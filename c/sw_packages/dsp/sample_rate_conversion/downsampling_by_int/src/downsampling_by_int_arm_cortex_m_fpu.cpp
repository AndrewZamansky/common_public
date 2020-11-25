/*
 * file :   downsampling_by_int_arm_cortex_m.c
 *
 */

#include "_project_typedefs.h"
#include "_project_defines.h"

#include "dsp_management_api.h"
#include "common_dsp_api.h"

#include "cpu_config.h"
#include "arm_math.h"
#include "downsampling_by_int.h"
#include "downsampling_by_int_api.h"
#include "fir_filter_api.h"
#include "fir_filter_internal_api.h"
extern "C" {
	#include "errors_api.h"
	#include "os_wrapper.h"
}

struct downsampling_by_int_t {
	void *p_filter_instance;
	float *p_filter_state;
	real_t *p_coefficients;
};



void downsampling_by_int_function(void *p_filter,
	real_t *in_buf, real_t *tmp_buf, size_t in_buff_len, real_t *out_buf)
{
	arm_fir_decimate_instance_f32 *p_arm_fir_decimate;
	struct downsampling_by_int_t *downsampling_by_int;

	downsampling_by_int = (struct downsampling_by_int_t *)p_filter;
	p_arm_fir_decimate = (arm_fir_decimate_instance_f32*)(
									downsampling_by_int->p_filter_instance);
	arm_fir_decimate_f32(p_arm_fir_decimate, in_buf, out_buf, in_buff_len);
}


/*  func : firs_free()
 */
void downsampling_by_int_free(void *pFilter)
{
	struct downsampling_by_int_t *p_filter;

	p_filter = (struct downsampling_by_int_t *)pFilter;
	os_safe_free(p_filter->p_filter_state);
	os_safe_free(p_filter->p_filter_instance);
	os_safe_free(p_filter->p_coefficients);
	os_safe_free(pFilter);
}


/*   func : downsampling_by_int_create()
 */
void downsampling_by_int_create(
		struct downsampling_by_int_instance_t *handle,
		struct fir_filter_api_set_params_t *fir_set_params)
{
	arm_status	status;
	size_t predefined_data_block_size;
	size_t number_of_filter_coefficients;
	struct downsampling_by_int_t *p_downsampling_by_int;
	arm_fir_decimate_instance_f32* p_filter_instance;
	float *p_filter_state;
	real_t *p_coefficients;
	struct fir_filter_api_set_coefficients_by_params_t  *coeff_by_params;

	p_downsampling_by_int = (struct downsampling_by_int_t *)malloc(
										sizeof(struct downsampling_by_int_t));
	errors_api_check_if_malloc_succeed(p_downsampling_by_int);

	p_filter_instance = (arm_fir_decimate_instance_f32*)malloc(
										sizeof(arm_fir_decimate_instance_f32));
	errors_api_check_if_malloc_succeed(p_filter_instance);

	p_downsampling_by_int->p_filter_instance = p_filter_instance;

	predefined_data_block_size = fir_set_params->predefined_data_block_size;
	number_of_filter_coefficients =
			fir_set_params->number_of_filter_coefficients;
	p_filter_state = (float*)malloc(sizeof(float) *
			(number_of_filter_coefficients + predefined_data_block_size - 1));
	errors_api_check_if_malloc_succeed(p_filter_state);
	p_downsampling_by_int->p_filter_state = p_filter_state;

	p_coefficients = (real_t *)malloc(
			number_of_filter_coefficients * sizeof(real_t));
	errors_api_check_if_malloc_succeed(p_coefficients);
	p_downsampling_by_int->p_coefficients = p_coefficients;

	coeff_by_params = &fir_set_params->coeff_by_params;
	fir_coefficients_calculation(coeff_by_params->filter_mode,
			coeff_by_params->fc, coeff_by_params->A_stop,
			coeff_by_params->dfc, coeff_by_params->sample_rate_Hz,
			p_coefficients, number_of_filter_coefficients);

	status = arm_fir_decimate_init_f32(p_filter_instance,
			(uint16_t)number_of_filter_coefficients,
			(uint8_t)handle->factor,
			p_coefficients, p_filter_state, predefined_data_block_size);

	if (ARM_MATH_SUCCESS != status)
	{
		downsampling_by_int_free(p_downsampling_by_int);
		p_downsampling_by_int = NULL;
	}
	handle->p_downsampling_by_int_filter = p_downsampling_by_int;
}
