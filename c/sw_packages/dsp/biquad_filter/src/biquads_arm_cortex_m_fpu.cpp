/*
 *
 * file :   biquads.c
 *
 *
 */



/********  includes *********************/
#include "_project_typedefs.h"
#include "_project_defines.h"
extern "C" {
#include "errors_api.h"
#include "os_wrapper.h"
}

#include "dsp_management_api.h"

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

/********  defines *********************/


/********  types  *********************/

struct biquads_cascading_filter_t
{
	void *pFilterParams;
	float *p_filter_state;
};



/********  externals *********************/


/********  local defs *********************/


/**********   external variables    **************/



/***********   local variables    **************/

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
 *     y[n] = b0 * x[n] + b1 * x[n-1] + b2 * x[n-2] + a1 * y[n-1] + a2 * y[n-2]
 *
 *    {b10, b11, b12, a11, a12, b20, b21, b22, a21, a22, ...}
 */
void *biquads_alloc(uint8_t num_of_stages, float *pCoeffs )
{
	struct biquads_cascading_filter_t *p_biquads_cascading_filter;
	arm_biquad_cascade_df2T_instance_f32* p_arm_filter_inst;
	float* p_filter_state;

	p_biquads_cascading_filter =
			(struct biquads_cascading_filter_t *)os_safe_malloc(
									sizeof(struct biquads_cascading_filter_t));
	errors_api_check_if_malloc_secceed(p_biquads_cascading_filter);

	p_arm_filter_inst =
			(arm_biquad_cascade_df2T_instance_f32*)os_safe_malloc(
								sizeof(arm_biquad_cascade_df2T_instance_f32));
	errors_api_check_if_malloc_secceed(p_arm_filter_inst);
	p_biquads_cascading_filter->pFilterParams = p_arm_filter_inst;

	p_filter_state = (float*)os_safe_malloc(
			NUM_OF_STATES_PER_STAGE * num_of_stages * sizeof(float));
	errors_api_check_if_malloc_secceed(p_filter_state);
	p_biquads_cascading_filter->p_filter_state = p_filter_state;

	arm_biquad_cascade_df2T_init_f32(p_arm_filter_inst,
					num_of_stages, pCoeffs,  p_filter_state );

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



/*   func : biquads_calculation()
 *    The coefficients are stored in the array pCoeffs in the following order:
 *
 *     y[n] = b0 * x[n] + b1 * x[n-1] + b2 * x[n-2] + a1 * y[n-1] + a2 * y[n-2]
 *
 *    {b10, b11, b12, a11, a12, b20, b21, b22, a21, a22, ...}
 */
void biquads_coefficients_calculation(biquads_filter_mode_t filter_mode,
		float FreqC,float QValue,float Gain_dB,
		float SamplingRate, float *pCoeffs )
{
	biquads_coefficients_calculation_common(filter_mode,
			 FreqC, QValue, Gain_dB, SamplingRate,  pCoeffs);

	pCoeffs[3] = -pCoeffs[3];
	pCoeffs[4] = -pCoeffs[4];
}
