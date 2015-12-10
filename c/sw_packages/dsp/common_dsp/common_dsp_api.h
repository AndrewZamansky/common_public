
#ifndef _common_dsp_API_H_
#define _common_dsp_API_H_

#include "common_dsp_config.h"
#include "src/_common_dsp_prerequirements_check.h" // should be after {equalizer_config.h,dev_managment_api.h}

/*****************  defines  **************/


/**********  define API  types ************/
typedef enum _biquads_filter_mode_t {
    BIQUADS_TRANSPARENT_MODE,
    BIQUADS_LOWPASS_MODE_1_POLE,
    BIQUADS_LOWPASS_MODE_2_POLES,
    BIQUADS_HIGHPASS_MODE_1_POLE,
    BIQUADS_HIGHPASS_MODE_2_POLES,
    BIQUADS_PEAK_MODE,
    BIQUADS_LOWSHELF_MODE,
    BIQUADS_HIGHSHELF_MODE,

} biquads_filter_mode_t;




/**********  define API  functions  ************/

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

#endif
