/*
 *
 * file :   biquads.c
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
#include "dsp_management_internal_api.h"

#include "_project.h"
#include "cpu_config.h"

#if defined(__GNUC__) && !defined(__ARMCC_VERSION)
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wsign-compare"
#endif
//extern "C" {
#include "arm_math.h"
//}
#if defined(__GNUC__) && !defined(__ARMCC_VERSION)
	#pragma GCC diagnostic pop
#endif

#include "biquad_filter.h"
#include "biquad_filter_api.h"


#define NUM_OF_STATES_PER_STAGE   2

struct biquads_cascading_filter_t {
	void *pFilterParams;
	float *p_filter_state;
};


void biquads_cascading_filter(void *pFilter,
		float *apIn, float *apOut, size_t buff_len)
{
	struct biquads_cascading_filter_t* filter;
	arm_biquad_cascade_df2T_instance_f32 *filter_params;

	filter = (struct biquads_cascading_filter_t*)pFilter;
	filter_params =
			(arm_biquad_cascade_df2T_instance_f32 *)filter->pFilterParams;
	arm_biquad_cascade_df2T_f32(filter_params, apIn, apOut, buff_len);
}


/*   func : biquads_alloc()
 *    The coefficients are stored in the array pCoeffs in the following order:
 *
 *     y[n] = b0 * x[n] + b1 * x[n-1] + b2 * x[n-2] - a1 * y[n-1] - a2 * y[n-2]
 *
 *    {b10, b11, b12, a11, a12, b20, b21, b22, a21, a22, ...}
 *    pay attention: an1 and an2 are negative
 */
void *biquads_alloc(uint8_t num_of_stages, float *biquad_bands_coeffs )
{
	struct biquads_cascading_filter_t *p_biquads_cascading_filter;
	arm_biquad_cascade_df2T_instance_f32* p_arm_filter_inst;
	float* p_filter_state;

	p_biquads_cascading_filter =
			(struct biquads_cascading_filter_t *)os_safe_malloc(
									sizeof(struct biquads_cascading_filter_t));
	errors_api_check_if_malloc_succeed(p_biquads_cascading_filter);

	p_arm_filter_inst =
			(arm_biquad_cascade_df2T_instance_f32*)os_safe_malloc(
								sizeof(arm_biquad_cascade_df2T_instance_f32));
	errors_api_check_if_malloc_succeed(p_arm_filter_inst);
	p_biquads_cascading_filter->pFilterParams = p_arm_filter_inst;

	p_filter_state = (float*)os_safe_malloc(
			NUM_OF_STATES_PER_STAGE * num_of_stages * sizeof(float));
	errors_api_check_if_malloc_succeed(p_filter_state);
	p_biquads_cascading_filter->p_filter_state = p_filter_state;

	arm_biquad_cascade_df2T_init_f32(p_arm_filter_inst,
					num_of_stages, biquad_bands_coeffs, p_filter_state );

	return p_biquads_cascading_filter;
}


/*  func : biquads_free()
 */
void biquads_free(void *pFilter)
{
	struct biquads_cascading_filter_t* p_biquads_cascading_filter;

	if (NULL == pFilter) return;

	p_biquads_cascading_filter = (struct biquads_cascading_filter_t*)pFilter;

	os_safe_free(p_biquads_cascading_filter->p_filter_state);
	os_safe_free(p_biquads_cascading_filter->pFilterParams);
	os_safe_free(p_biquads_cascading_filter);
}

void biquads_coefficients_assign(void *pFilter,
		real_t b0, real_t b1, real_t b2, real_t a1, real_t a2,
		real_t *biquad_bands_coeffs, uint8_t band_num)
{
	real_t *curr_coeffs;

	curr_coeffs = &biquad_bands_coeffs[5 * band_num];

	curr_coeffs[0] = b0;
	curr_coeffs[1] = b1;
	curr_coeffs[2] = b2;
	curr_coeffs[3] = -a1;
	curr_coeffs[4] = -a2;
}


void biquads_coefficients_assign_custom_fix_point(
		void *pFilter, int16_t num_of_fraction_bits,
		int32_t b0, int32_t b1, int32_t b2, int32_t a1, int32_t a2,
		real_t *biquad_bands_coeffs, uint8_t band_num)
{
	CRITICAL_ERROR("custom fix point coeff not implemented yet");
}

