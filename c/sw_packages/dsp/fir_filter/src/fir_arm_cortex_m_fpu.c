/*
 *
 * file :   firs.c
 *
 */
#include "_project_typedefs.h"
#include "_project_defines.h"
extern "C" {
#include "errors_api.h"
}

#include "dsp_management_api.h"
#include "common_dsp_api.h"

#include "cpu_config.h"
#include "arm_math.h"
#include "fir_filter.h"
#include "fir_filter_api.h"

#include "_fir_filter_prerequirements_check.h"


struct fir_filter_t {
	void *p_filter_instance;
	float *p_fir_filter_state;
};


void fir_filter_function(
		void *pFilter, float *apIn, float *apOut, size_t buff_len)
{
	arm_fir_instance_f32 *filter_params;
	struct fir_filter_t * fir_filter;

	fir_filter = (struct fir_filter_t *)pFilter;
	filter_params = (arm_fir_instance_f32*)(fir_filter->p_filter_instance);
	arm_fir_f32(filter_params, apIn, apOut, buff_len);
}




/*   func : fir_alloc()
 *    The coefficients are stored in the array pCoeffs in the following order:
 *
 *    y[n] = b[0] * x[n] + b[1] * x[n-1] + b[2] * x[n-2] + ...+ b[numTaps-1] * x[n-numTaps+1]
 *
 *    {b[numTaps-1], b[numTaps-2], b[N-2], ..., b[1], b[0]}
 */
void *fir_alloc(size_t number_of_filter_coefficients,
		float *p_coefficients ,size_t predefined_data_block_size)
{
	struct fir_filter_t *p_fir_filter;
	arm_fir_instance_f32* p_filter_instance;
	float *p_fir_filter_state;

	p_fir_filter=(struct fir_filter_t *)malloc(sizeof(struct fir_filter_t));
	errors_api_check_if_malloc_succeed(p_fir_filter);

	p_filter_instance = malloc(sizeof(arm_fir_instance_f32));
	errors_api_check_if_malloc_succeed(p_filter_instance);
	p_fir_filter->p_filter_instance = p_filter_instance;

	p_fir_filter_state = (float*)malloc(sizeof(float) *
			(number_of_filter_coefficients + predefined_data_block_size - 1));
	errors_api_check_if_malloc_succeed(p_fir_filter_state);
	p_fir_filter->p_fir_filter_state = p_fir_filter_state;

	arm_fir_init_f32 (p_filter_instance,
			(uint16_t)number_of_filter_coefficients,
			p_coefficients, p_fir_filter_state, predefined_data_block_size);
	return p_fir_filter;
}


/*  func : firs_free()
 */
void firs_free(void *pFilter )
{
	free(((struct fir_filter_t *)pFilter)->p_fir_filter_state);
	free(((struct fir_filter_t *)pFilter)->p_filter_instance);
	free(pFilter);
}
