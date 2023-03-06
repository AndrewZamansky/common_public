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

#include "biquad_filter.h"
#include "biquad_filter_api.h"


#define USE_DIRECT_FORM_I

#if !defined(USE_DIRECT_FORM_I)
	#define NUM_OF_STATES_PER_STAGE	2
#else
	#define NUM_OF_STATES_PER_STAGE	4
#endif


struct biquads_cascading_filter_t
{
	uint8_t numOfStages;
	real_t *  pCoeffs;

	// should be allocated numOfStages * NUM_OF_STATES_PER_STAGE
	real_t *  p_states;
};


/*   func : biquads_filter()
 *    The coefficients are stored in the array pCoeffs in the following order:
 *
 *     y[n] = b0 * x[n] + b1 * x[n-1] + b2 * x[n-2] + a1 * y[n-1] + a2 * y[n-2]
 *
 *    {b10, b11, b12, a11, a12, b20, b21, b22, a21, a22, ...}
 */
#if defined(CONFIG_DSP_REAL_NUMBER_FORMAT_FLOATING_POINT)

	static void biquads_filter(real_t *p_states, real_t *p_coeffs,
			real_t *apIn, real_t *apOut, size_t buff_len)
	{
		uint16_t count;
		real_t state1, state2;
		real_t A1, A2, B0, B1, B2;
		real_t curr_x;
		real_t out_tmp;
		real_t tmp;

		state1 = p_states[0];
		state2 = p_states[1];

		B0 = p_coeffs[0];
		B1 = p_coeffs[1];
		B2 = p_coeffs[2];
		A1 = p_coeffs[3];
		A2 = p_coeffs[4];

		for(count = 0; count < buff_len; count++)
		{
			curr_x = *apIn++;

			out_tmp = B0 * curr_x ;
			out_tmp += state1;
			state1 = B1 * curr_x;
			tmp	=  A1 * out_tmp;
			state1 -= tmp;
			state1 += state2;
			state2 = B2 * curr_x;
			tmp =  A2 * out_tmp;
			state2 -= tmp;

			*apOut++ = out_tmp;
		}

		p_states[0] = state1;
		p_states[1] = state2;
	}

#else

	static void biquads_filter(real_t *p_states, real_t *p_coeffs,
			real_t *apIn, real_t *apOut, size_t buff_len)
	{
		uint16_t count;
		fix16_t x1, x2, y1, y2;
		fix16_t A1, A2, B0, B1, B2;
		fix16_t curr_x;
		fix16_t out_tmp;
		int64_t tmp;

		x1 = p_states[0];
		x2 = p_states[1];
		y1 = p_states[2];
		y2 = p_states[3];

		B0 = p_coeffs[0];
		B1 = p_coeffs[1];
		B2 = p_coeffs[2];
		A1 = p_coeffs[3];
		A2 = p_coeffs[4];

		for(count = 0; count < buff_len; count++)
		{
			curr_x = *apIn++;

			tmp = (int64_t)B0 * curr_x;
			out_tmp = tmp >> 16;
			tmp = (int64_t)B1 * x1;
			out_tmp += tmp >> 16;
			tmp = (int64_t)B2 * x2;
			out_tmp += tmp >> 16;
			tmp = (int64_t)A1 * y1;
			out_tmp += tmp >> 16;
			tmp = (int64_t)A2 * y2;
			out_tmp += tmp >> 16;

			x2 = x1;
			x1 = curr_x;
			y2 = y1;
			y1 = out_tmp;
			*apOut++ = out_tmp;
		}

		p_states[0] = x1;
		p_states[1] = x2;
		p_states[2] = y1;
		p_states[3] = y2;
	}

#endif


void biquads_cascading_filter(void *pFilter,
		real_t *apIn, real_t *apOut, size_t buff_len)
{
	struct biquads_cascading_filter_t  *p_biquads_cascading_filter;
	real_t *p_states;
	uint8_t currStage;
	uint8_t numOfStages ;
	real_t *pCoeffs;

	p_biquads_cascading_filter = (struct biquads_cascading_filter_t  *)pFilter;
	pCoeffs = p_biquads_cascading_filter->pCoeffs;
	numOfStages = p_biquads_cascading_filter->numOfStages;
	p_states = p_biquads_cascading_filter->p_states;

	for (currStage = 0 ; currStage < numOfStages ; currStage++)
	{
		biquads_filter(&p_states[currStage * NUM_OF_STATES_PER_STAGE],
						&pCoeffs[ 5 * currStage], apIn, apOut, buff_len);
	}
}


/*   func : biquads_alloc()
 *    The coefficients are stored in the array pCoeffs in the following order:
 *
 *     y[n] = b0 * x[n] + b1 * x[n-1] + b2 * x[n-2] + a1 * y[n-1] + a2 * y[n-2]
 *
 *    {b10, b11, b12, a11, a12, b20, b21, b22, a21, a22, ...}
 */
void *biquads_alloc(uint8_t num_of_stages, real_t *biquad_bands_coeffs )
{
	struct biquads_cascading_filter_t *p_biquads_cascading_filter;
	real_t *p_states;
	size_t size_of_states;

	p_biquads_cascading_filter =
		(struct biquads_cascading_filter_t *)os_safe_malloc(
							sizeof(struct biquads_cascading_filter_t));
	errors_api_check_if_malloc_succeed(p_biquads_cascading_filter);

	size_of_states = NUM_OF_STATES_PER_STAGE * num_of_stages * sizeof(real_t);
	p_states = (real_t *)malloc(size_of_states);
	errors_api_check_if_malloc_succeed(p_states);
	memset((uint8_t*)p_states, 0, size_of_states);

	p_biquads_cascading_filter->p_states = p_states;
	p_biquads_cascading_filter->numOfStages = num_of_stages;
	p_biquads_cascading_filter->pCoeffs = biquad_bands_coeffs;

	return p_biquads_cascading_filter;
}


/*  func : biquads_free()
 */
void biquads_free(void *pFilter)
{
	struct biquads_cascading_filter_t  *p_biquads_cascading_filter;

	if (NULL == pFilter) return;

	p_biquads_cascading_filter = (struct biquads_cascading_filter_t  *)pFilter;
	os_safe_free( p_biquads_cascading_filter->p_states);
	os_safe_free(p_biquads_cascading_filter);
}


/*   func : biquads_coefficients_assign()
 *    The coefficients are stored in the array pCoeffs in the following order:
 *
 *     y[n] = b0 * x[n] + b1 * x[n-1] + b2 * x[n-2] + a1 * y[n-1] + a2 * y[n-2]
 *
 *    {b10, b11, b12, a11, a12, b20, b21, b22, a21, a22, ...}
 */
void biquads_coefficients_assign(void *pFilter,
		real_t b0, real_t b1, real_t b2, real_t a1, real_t a2,
		real_t *biquad_bands_coeffs, uint8_t band_num)
{
	real_t *curr_coeffs;

	curr_coeffs = &biquad_bands_coeffs[5 * band_num];

	curr_coeffs[0] = b0;
	curr_coeffs[1] = b1;
	curr_coeffs[2] = b2;
	curr_coeffs[3] = a1;
	curr_coeffs[4] = a2;
}


void biquads_coefficients_assign_custom_fix_point(
		void *pFilter, int16_t num_of_fraction_bits,
		int32_t b0, int32_t b1, int32_t b2, int32_t a1, int32_t a2,
		real_t *biquad_bands_coeffs, uint8_t band_num)
{
	CRITICAL_ERROR("custom fix point coeff not implemented yet");
}
