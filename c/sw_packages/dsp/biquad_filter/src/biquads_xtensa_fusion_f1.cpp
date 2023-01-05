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


//extern "C" {
#include "NatureDSP_Signal.h"
//}



#include "biquad_filter.h"
#include "biquad_filter_api.h"


#define NUM_OF_STATES_PER_STAGE   2

struct biquads_cascading_filter_t {
	void *p_filter_mem;
	int32_t *coef_sos; // coefficients
	int16_t *coef_g;  // scale factor
	void *scratch_memory_area;
	bqriir32x32_df1_handle_t  bqriir32x32_df1_handle;
};

static int32_t in[512];
static int32_t out[512];
void biquads_cascading_filter(void *pFilter,
		real_t *apIn, real_t *apOut, size_t buff_len)
{
	struct biquads_cascading_filter_t* filter;
	bqriir32x32_df1_handle_t  bqriir32x32_df1_handle;
	size_t i;

	for (i = 0; i < buff_len; i++)
	{
		in[i] = ((fix16_t)apIn[i]) << 15;
	}

	filter = (struct biquads_cascading_filter_t*)pFilter;
	bqriir32x32_df1_handle = filter->bqriir32x32_df1_handle;

	bqriir32x32_df1(bqriir32x32_df1_handle,
			filter->scratch_memory_area, in, out, buff_len);

	for (i = 0; i < buff_len; i++)
	{
		apOut[i] = (fix16_t)(out[i] / 0x7ffff);
	}
}


/*   func : biquads_alloc()
 *    The coefficients are stored in the array pCoeffs in the following order:
 *
 *     y[n] = b0 * x[n] + b1 * x[n-1] + b2 * x[n-2] - a1 * y[n-1] - a2 * y[n-2]
 *
 *    {b10, b11, b12, a11, a12, b20, b21, b22, a21, a22, ...}
 *    pay attention: an1 and an2 are negative
 */
void *biquads_alloc(uint8_t num_of_stages, real_t *pCoeffs )
{
	size_t alloc_size;
	void *p_filter_mem;
	struct biquads_cascading_filter_t *p_biquads_cascading_filter;
	bqriir32x32_df1_handle_t  bqriir32x32_df1_handle;
	int32_t *coef_sos; // coefficients
	int16_t *coef_g;  // scale factor
	void *scratch_memory_area;
	uint8_t i;

	p_biquads_cascading_filter =
			(struct biquads_cascading_filter_t *)os_safe_malloc(
									sizeof(struct biquads_cascading_filter_t));
	errors_api_check_if_malloc_succeed(p_biquads_cascading_filter);

	alloc_size = bqriir32x32_df1_alloc(num_of_stages);

	p_filter_mem = os_safe_malloc(alloc_size);
	errors_api_check_if_malloc_succeed(p_filter_mem);
	p_biquads_cascading_filter->p_filter_mem = p_filter_mem;

	coef_sos = (int32_t*)os_safe_malloc(num_of_stages * 5 * sizeof(int32_t));
	errors_api_check_if_malloc_succeed(coef_sos);
	p_biquads_cascading_filter->coef_sos = coef_sos;
	for (i = 0; i < (num_of_stages * 5); i++)
	{
		coef_sos[i] = ((fix16_t)pCoeffs[i]) << 15;
	}

	coef_g = (int16_t*)os_safe_malloc(num_of_stages * sizeof(int16_t));
	errors_api_check_if_malloc_succeed(coef_g);
	p_biquads_cascading_filter->coef_g = coef_g;
	for (i = 0; i < num_of_stages; i++)
	{
		coef_g[i] = 1;
	}


	scratch_memory_area =
			os_safe_malloc(BQRIIR32X32_DF1_SCRATCH_SIZE(512, num_of_stages));
	errors_api_check_if_malloc_succeed(scratch_memory_area);
	p_biquads_cascading_filter->scratch_memory_area = scratch_memory_area;

	bqriir32x32_df1_handle = bqriir32x32_df1_init(p_filter_mem,
			num_of_stages, coef_sos, coef_g, 1);
	p_biquads_cascading_filter->bqriir32x32_df1_handle = bqriir32x32_df1_handle;

	return p_biquads_cascading_filter;
}


/*  func : biquads_free()
 */
void biquads_free(void *pFilter)
{
	struct biquads_cascading_filter_t* p_biquads_cascading_filter;

	if (NULL == pFilter) return;

	p_biquads_cascading_filter = (struct biquads_cascading_filter_t*)pFilter;

	os_safe_free(p_biquads_cascading_filter->scratch_memory_area);
	os_safe_free(p_biquads_cascading_filter->coef_g);
	os_safe_free(p_biquads_cascading_filter->coef_sos);
	os_safe_free(p_biquads_cascading_filter->p_filter_mem);
	os_safe_free(p_biquads_cascading_filter);
}

static real_t zero = 0;

/*   func : biquads_calculation()
 *    The coefficients are stored in the array pCoeffs in the following order:
 *
 *     y[n] = b0 * x[n] + b1 * x[n-1] + b2 * x[n-2] - a1 * y[n-1] - a2 * y[n-2]
 *
 *    {b10, b11, b12, a11, a12, b20, b21, b22, a21, a22, ...}
 *    pay attention: an1 and an2 are negative
 */
void biquads_coefficients_calculation(enum biquads_filter_mode_e filter_mode,
		real_t FreqC, real_t QValue, real_t Gain_dB,
		real_t SamplingRate, real_t *pCoeffs )
{
	biquads_coefficients_calculation_common(filter_mode,
			 FreqC, QValue, Gain_dB, SamplingRate,  pCoeffs);

	pCoeffs[3] = zero - pCoeffs[3];
	pCoeffs[4] = zero - pCoeffs[4];
}

void biquads_coefficients_assign(real_t b0, real_t b1, real_t b2,
		real_t a1, real_t a2, real_t *pCoeffs)
{
	pCoeffs[0] = b0;
	pCoeffs[1] = b1;
	pCoeffs[2] = b2;
	pCoeffs[3] = zero - a1;
	pCoeffs[4] = zero - a2;
}
