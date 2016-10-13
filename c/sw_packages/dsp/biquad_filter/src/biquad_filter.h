/*
 * file : BIQUAD_FILTER.h
 *
 *
 *
 *
 *
 *
 */

#ifndef _BIQUAD_FILTER_H
#define _BIQUAD_FILTER_H

#include "src/_biquad_filter_prerequirements_check.h"


/***************   typedefs    *******************/

typedef struct _BandCoeffs_t
{
	float b0, b1 , b2 , a1 , a2;
} BandCoeffs_t;


typedef struct {

	void *pBiquadFilter   ;
	size_t num_of_bands ;
	BandCoeffs_t *pCoeffs;
	biquad_filter_api_band_set_params_t band_set_params;
} BIQUAD_FILTER_Instance_t;


/*   func : biquads_alloc()
 *    The coefficients are stored in the array pCoeffs in the following order:
 *
 *     y[n] = b0 * x[n] + b1 * x[n-1] + b2 * x[n-2] + a1 * y[n-1] + a2 * y[n-2]
 *
 *    {b10, b11, b12, a11, a12, b20, b21, b22, a21, a22, ...}
 */
void *biquads_alloc(uint8_t num_of_stages, float *  	pCoeffs );

/*  func : biquads_free()
 */
void biquads_free(void *pFilter,uint8_t num_of_stages, float *  	pCoeffs );

void biquads_cascading_filter(void *pFilter,float *pin,float *pOut,size_t buff_len);

void biquads_calculation(biquads_filter_mode_t filter_mode,
		float FreqC,float QValue,float Gain_dB,
		float SamplingRate, float *pCoeffs );



#endif /* */
