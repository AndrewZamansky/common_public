/*
 * file : biquad_filter.h
 *
 *
 */

#ifndef _BIQUAD_FILTER_H
#define _BIQUAD_FILTER_H

#include "biquad_filter_api.h"


struct biquads_filter_t {
	void *pBiquadFilter   ;
	size_t num_of_bands ;
	real_t *biquad_bands_coeffs; /* b0,b1,b2,a1,a2 ; b0,b1,b2,a1,a2 ....*/
	struct biquad_filter_api_band_set_params_t *band_set_params;
};


/*   func : biquads_alloc()
 *    The coefficients are stored in the array pCoeffs in the following order:
 *
 *     y[n] = b0 * x[n] + b1 * x[n-1] + b2 * x[n-2] + a1 * y[n-1] + a2 * y[n-2]
 *
 *    {b10, b11, b12, a11, a12, b20, b21, b22, a21, a22, ...}
 */
void *biquads_alloc(uint8_t num_of_stages, real_t *pCoeffs );

void biquads_free(void *pFilter );

void biquads_cascading_filter(void *pFilter,
		real_t *pin, real_t *pOut, size_t buff_len);

void biquads_coefficients_calculation_common(
		enum biquads_filter_mode_e filter_mode,
		real_t FreqC, real_t QValue, real_t Gain_dB,
		real_t SamplingRate, real_t *pCoeffs);


void biquads_coefficients_assign(void *pFilter,
		real_t b0, real_t b1, real_t b2, real_t a1, real_t a2,
		real_t *biquad_bands_coeffs, uint8_t band_num);

void biquads_coefficients_assign_custom_fix_point(
		void *pFilter, int16_t num_of_fraction_bits,
		int32_t b0, int32_t b1, int32_t b2, int32_t a1, int32_t a2,
		real_t *biquad_bands_coeffs, uint8_t band_num);

#endif
