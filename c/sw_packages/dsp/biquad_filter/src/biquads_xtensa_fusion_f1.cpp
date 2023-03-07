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
	uint8_t num_of_stages;
	void *scratch_memory_area;
	bqriir32x32_df1_handle_t  bqriir32x32_df1_handle;
};

static real_t zero = 0;

static int32_t in[512];
static int32_t out[512];


void biquads_cascading_filter(void *pFilter,
		real_t *apIn, real_t *apOut, size_t buff_len)
{
	struct biquads_cascading_filter_t* filter;
	bqriir32x32_df1_handle_t  bqriir32x32_df1_handle;
	size_t i;

	/* bqriir32x32_df1 expect coefficients in Q0.31 format*/
	/* on FusionF1 shift on signed int extend sign bit */
	for (i = 0; i < buff_len; i++)
	{
		in[i] = ((fix16_t)apIn[i]) << 15;
	}

	filter = (struct biquads_cascading_filter_t*)pFilter;
	bqriir32x32_df1_handle = filter->bqriir32x32_df1_handle;

	bqriir32x32_df1_nuvoton(bqriir32x32_df1_handle,
			filter->scratch_memory_area, out, in, buff_len);

	/* convert back to Q15.16 */
	/* on FusionF1 shift on signed int extend sign bit */
	for (i = 0; i < buff_len; i++)
	{
		apOut[i] = (fix16_t)(out[i] >> 15);
	}
}


static bqriir32x32_df1_handle_t set_filter(
		struct biquads_cascading_filter_t *p_biquads_cascading_filter,
		real_t *biquad_bands_coeffs, uint8_t do_init_states)
{
	int32_t *coef_sos; // coefficients
	int16_t *coef_g;  // scale factor
	uint8_t num_of_stages;
	uint8_t i;

	coef_sos = p_biquads_cascading_filter->coef_sos;
	coef_g = p_biquads_cascading_filter->coef_g;
	num_of_stages = p_biquads_cascading_filter->num_of_stages;

	/* bqriir32x32_df1 expect coefficients in Q1.30 format*/
	/* on FusionF1 shift on signed int extend sign bit */
	for (i = 0; i < (num_of_stages * 5); i++)
	{
		coef_sos[i] = ((fix16_t)biquad_bands_coeffs[i]) << 14;
	}

	for (i = 0; i < num_of_stages; i++)
	{
		coef_g[i] = 0x7fff; // coef_g is Q0.15 so 0x7fff is almost 1
	}
	return bqriir32x32_df1_init_nuvoton(
			p_biquads_cascading_filter->p_filter_mem,
			num_of_stages, coef_sos, coef_g, 0, do_init_states);
}


/*   func : biquads_alloc()
 *    The coefficients are stored in the array pCoeffs in the following order:
 *
 *     y[n] = b0 * x[n] + b1 * x[n-1] + b2 * x[n-2] - a1 * y[n-1] - a2 * y[n-2]
 *
 *    {b10, b11, b12, a11, a12, b20, b21, b22, a21, a22, ...}
 *    pay attention: an1 and an2 are negative
 */
void *biquads_alloc(uint8_t num_of_stages, real_t *biquad_bands_coeffs )
{
	size_t alloc_size;
	void *p_filter_mem;
	struct biquads_cascading_filter_t *p_biquads_cascading_filter;
	bqriir32x32_df1_handle_t  bqriir32x32_df1_handle;
	void *scratch_memory_area;
	int32_t *coef_sos; // coefficients
	int16_t *coef_g;  // scale factor

	p_biquads_cascading_filter =
			(struct biquads_cascading_filter_t *)os_safe_malloc(
									sizeof(struct biquads_cascading_filter_t));
	errors_api_check_if_malloc_succeed(p_biquads_cascading_filter);

	p_biquads_cascading_filter->num_of_stages = num_of_stages;

	alloc_size = bqriir32x32_df1_alloc(num_of_stages);

	p_filter_mem = os_safe_malloc(alloc_size);
	errors_api_check_if_malloc_succeed(p_filter_mem);
	p_biquads_cascading_filter->p_filter_mem = p_filter_mem;

	coef_sos = (int32_t*)os_safe_malloc(num_of_stages * 5 * sizeof(int32_t));
	errors_api_check_if_malloc_succeed(coef_sos);
	p_biquads_cascading_filter->coef_sos = coef_sos;

	coef_g = (int16_t*)os_safe_malloc(num_of_stages * sizeof(int16_t));
	errors_api_check_if_malloc_succeed(coef_g);
	p_biquads_cascading_filter->coef_g = coef_g;

	scratch_memory_area =
			os_safe_malloc(BQRIIR32X32_DF1_SCRATCH_SIZE(512, num_of_stages));
	errors_api_check_if_malloc_succeed(scratch_memory_area);
	p_biquads_cascading_filter->scratch_memory_area = scratch_memory_area;

	bqriir32x32_df1_handle =
			set_filter(p_biquads_cascading_filter, biquad_bands_coeffs, 1);
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


/*   func : biquads_coefficients_assign()
 *    The coefficients are stored in the array pCoeffs in the following order:
 *
 *     y[n] = b0 * x[n] + b1 * x[n-1] + b2 * x[n-2] - a1 * y[n-1] - a2 * y[n-2]
 *
 *    {b10, b11, b12, a11, a12, b20, b21, b22, a21, a22, ...}
 */
void biquads_coefficients_assign(void *pFilter,
		real_t b0, real_t b1, real_t b2, real_t a1, real_t a2,
		real_t *biquad_bands_coeffs, uint8_t band_num)
{
	real_t *curr_coeffs;
	void *p_filter_mem;
	struct biquads_cascading_filter_t* p_biquads_cascading_filter;


	curr_coeffs = &biquad_bands_coeffs[5 * band_num];
	curr_coeffs[0] = b0;
	curr_coeffs[1] = b1;
	curr_coeffs[2] = b2;
	curr_coeffs[3] = zero - a1;
	curr_coeffs[4] = zero - a2;

	p_biquads_cascading_filter = (struct biquads_cascading_filter_t*)pFilter;
	p_filter_mem = p_biquads_cascading_filter->p_filter_mem;

	set_filter(p_biquads_cascading_filter, biquad_bands_coeffs, 0);
}

void biquads_coefficients_assign_custom_fix_point(
		void *pFilter, int16_t num_of_fraction_bits,
		int32_t b0, int32_t b1, int32_t b2, int32_t a1, int32_t a2,
		real_t *biquad_bands_coeffs, uint8_t band_num)
{
	struct biquads_cascading_filter_t* p_biquads_cascading_filter;
	int32_t *coef_sos; // coefficients
	int32_t *curr_coef_sos; // coefficients
	int16_t *coef_g;  // scale factor
	uint8_t num_of_stages;
	uint8_t i;
	real_t *curr_coeffs;
	int16_t size_of_shift;

	curr_coeffs = &biquad_bands_coeffs[5 * band_num];

	/* on FusionF1 shift on signed int exptend sign bit */
	if (num_of_fraction_bits >= 16)
	{
		size_of_shift = num_of_fraction_bits - 16;
		curr_coeffs[0] = b0 >> size_of_shift;
		curr_coeffs[1] = b1 >> size_of_shift;
		curr_coeffs[2] = b2 >> size_of_shift;
		curr_coeffs[3] = 0 - (a1 >> size_of_shift);
		curr_coeffs[4] = 0 - (a2 >> size_of_shift);
	}
	else
	{
		size_of_shift = 16 - num_of_fraction_bits;
		curr_coeffs[0] = b0 << size_of_shift;
		curr_coeffs[1] = b1 << size_of_shift;
		curr_coeffs[2] = b2 << size_of_shift;
		curr_coeffs[3] = 0 - (a1 << size_of_shift);
		curr_coeffs[4] = 0 - (a2 << size_of_shift);
	}

	p_biquads_cascading_filter = (struct biquads_cascading_filter_t*)pFilter;

	coef_sos = p_biquads_cascading_filter->coef_sos;
	coef_g = p_biquads_cascading_filter->coef_g;
	num_of_stages = p_biquads_cascading_filter->num_of_stages;

	curr_coef_sos = &coef_sos[5 * band_num];
	/* bqriir32x32_df1 expect coefficients in Q1.30 format*/
	/* on FusionF1 shift on signed int extend sign bit */
	if (num_of_fraction_bits >= 30)
	{
		size_of_shift = num_of_fraction_bits - 30;
		curr_coef_sos[0] = b0 >> size_of_shift;
		curr_coef_sos[1] = b1 >> size_of_shift;
		curr_coef_sos[2] = b2 >> size_of_shift;
		curr_coef_sos[3] = 0 - (a1 >> size_of_shift);
		curr_coef_sos[4] = 0 - (a2 >> size_of_shift);
	}
	else
	{
		size_of_shift = 30 - num_of_fraction_bits;
		curr_coef_sos[0] = b0 << size_of_shift;
		curr_coef_sos[1] = b1 << size_of_shift;
		curr_coef_sos[2] = b2 << size_of_shift;
		curr_coef_sos[3] = 0 - (a1 << size_of_shift);
		curr_coef_sos[4] = 0 - (a2 << size_of_shift);
	}

	for (i = 0; i < num_of_stages; i++)
	{
		coef_g[i] = 0x7fff; // coef_g is Q0.15 so 0x7fff is almost 1
	}
	bqriir32x32_df1_init_nuvoton(p_biquads_cascading_filter->p_filter_mem,
			num_of_stages, coef_sos, coef_g, 0, 0);
}
