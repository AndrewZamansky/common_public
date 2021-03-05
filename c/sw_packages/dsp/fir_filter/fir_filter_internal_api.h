
#ifndef _fir_filter_internal_API_H_
#define _fir_filter_internal_API_H_

#include "dsp_management_api.h"
#include "dsp_management_internal_api.h"

/* func: fir_coefficients_calculation
* fc - requency where it happens
* Astop - stopband attenuation in dB,
* filter_mode - filter type
* dfc -  half-filter bandwidth (only for bandpass and notch)
* modified by WMXZ and DD4WH after
* Wheatley, M. (2011): CuteSDR Technical Manual. www.metronix.com,
* pages 118 - 120, FIR with Kaiser-Bessel Window
* assess required number of coefficients by
* numCoeffs = (Astop - 8.0) / (2.285 * TPI * normFtrans);
* electing high-pass, numCoeffs is forced to an even number for
* better frequency response
*
* return : coefficients in reversed order:
* {b[numTaps-1], b[numTaps-2], b[N-2], ..., b[1], b[0]}
*/
void fir_coefficients_calculation( enum fir_filter_mode_e filter_mode,
		real_t fc, real_t Astop, real_t dfc,
		real_t sample_rate, real_t *p_coeffs, size_t coeff_num);
#endif
