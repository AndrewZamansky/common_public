/*
 *
 * file :   firs.c
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

#include "cpu_config.h"
#include "arm_math.h"
#include "fir_filter.h"
#include "fir_filter_api.h"

struct fir_filter_arm_t {
	void *p_filter_instance;
	float *p_fir_filter_state;
};


void fir_filter_function(
		void *pFilter, real_t *apIn, real_t *apOut, size_t buff_len)
{
	arm_fir_instance_f32 *filter_params;
	struct fir_filter_arm_t * fir_filter;

	fir_filter = (struct fir_filter_arm_t *)pFilter;
	filter_params = (arm_fir_instance_f32*)(fir_filter->p_filter_instance);
	arm_fir_f32(filter_params, apIn, apOut, buff_len);
}




/*   func : fir_alloc()
 *    The coefficients are stored in the array pCoeffs in the following order:
 *
 *    y[n] = b[0] * x[n] + b[1] * x[n-1] +
 *              + b[2] * x[n-2] + ...+ b[numTaps-1] * x[n-numTaps+1]
 *
 *    {b[numTaps-1], b[numTaps-2], b[N-2], ..., b[1], b[0]}
 */
void *fir_alloc(size_t number_of_filter_coefficients,
		float *p_coefficients ,size_t expected_number_of_input_samples)
{
	struct fir_filter_arm_t *p_fir_filter;
	arm_fir_instance_f32* p_filter_instance;
	float *p_fir_filter_state;

	p_fir_filter = (struct fir_filter_arm_t *)os_safe_malloc(
									sizeof(struct fir_filter_arm_t));
	errors_api_check_if_malloc_succeed(p_fir_filter);

	p_filter_instance =
		(arm_fir_instance_f32*)os_safe_malloc(sizeof(arm_fir_instance_f32));
	errors_api_check_if_malloc_succeed(p_filter_instance);
	p_fir_filter->p_filter_instance = p_filter_instance;

	p_fir_filter_state = (float*)os_safe_malloc(sizeof(float) *
		(number_of_filter_coefficients + expected_number_of_input_samples - 1));
	errors_api_check_if_malloc_succeed(p_fir_filter_state);
	p_fir_filter->p_fir_filter_state = p_fir_filter_state;

	arm_fir_init_f32 (p_filter_instance,
		(uint16_t)number_of_filter_coefficients,
		p_coefficients, p_fir_filter_state, expected_number_of_input_samples);
	return p_fir_filter;
}


/*  func : fir_free()
 */
void fir_free(void *pFilter )
{
	if (NULL == pFilter) return;
	os_safe_free(((struct fir_filter_arm_t *)pFilter)->p_fir_filter_state);
	os_safe_free(((struct fir_filter_arm_t *)pFilter)->p_filter_instance);
	os_safe_free(pFilter);
}
