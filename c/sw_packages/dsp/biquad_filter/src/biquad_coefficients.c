/*
 *
 * file :   biquad_filter.c
 *
 *
 *
 *
 *
 */



/********  includes *********************/
#include "_project.h"

#include "_biquad_filter_prerequirements_check.h"


#include "biquad_filter_api.h" //place first to test that header file is self-contained
#include "biquad_filter.h"

#include "auto_init_api.h"
#include "math.h"

/********  defines *********************/


/********  types  *********************/

/********  externals *********************/


/********  exported variables *********************/



/**********   external variables    **************/



/***********   local variables    **************/



/*   func : biquads_coefficients_calculation_common()
 *    The coefficients are stored in the array pCoeffs in the following order:
 *
 *     y[n] = b0 * x[n] + b1 * x[n-1] + b2 * x[n-2] + a1 * y[n-1] + a2 * y[n-2]
 *
 *    {b10, b11, b12, a11, a12, b20, b21, b22, a21, a22, ...}
 */
void biquads_coefficients_calculation_common(biquads_filter_mode_t filter_mode,
		float FreqC,float QValue,float Gain_dB,
		float SamplingRate, float *pCoeffs )
{
	float norm,V,K	;
	float ftem , G_abs	;
	float b0,b1,b2,a1,a2,w0;
	float K_div_Q;

	b0 = 1.0	;

	ftem = FreqC/SamplingRate	;
    w0 = 2 * 3.1415962 * ftem;
	K = tan(3.1415962 * ftem)	;
	G_abs = (Gain_dB >= 0) ? Gain_dB : -Gain_dB	;
	V = pow(10,G_abs/20)	;

	K_div_Q = K/QValue;

	switch(filter_mode)
	{
		default :
		case BIQUADS_TRANSPARENT_MODE :
			a1 = a2 = b1 = b2 = 0	;
			break	;

		case BIQUADS_LOWPASS_MODE_1_POLE :
            a1 = -exp(-w0);
            a2 = 0;
            b0 = 1 + a1;
            b1 = 0;
            b2 = 0;
			break;

		case BIQUADS_LOWPASS_MODE_2_POLES :
			norm = 1/(1 + K_div_Q + K * K)	;
			b0 = K * K * norm	;
			b1 = 2 * b0	;
			b2 = b0	;
			a1 = 2 * (K * K - 1) * norm	;
			a2 = (1 - K_div_Q + K * K) * norm	;
			break;

		case BIQUADS_HIGHPASS_MODE_1_POLE :
            a1 = -exp(-w0);
            a2 = 0;
            b0 = (1 - a1) / 2;
            b1 = -b0;
            b2 = 0;
			break;

		case BIQUADS_HIGHPASS_MODE_2_POLES :
			norm = 1/(1 + K_div_Q + K * K)	;
			b0 = norm	;
			b1 = -2 * b0	;
			b2 = b0	;
			a1 = 2 * (K * K - 1) * norm	;
			a2 = (1 - K_div_Q + K * K) * norm	;
			break;

		case BIQUADS_BANDPASS_MODE :
			norm = 1/(1 + K_div_Q + K * K)	;
			b0 = K_div_Q *	norm;
			b1 = 0	;
			b2 = -b0	;
			a1 = 2 * (K * K - 1) * norm	;
			a2 = (1 - K_div_Q + K * K) * norm	;
			break;

		case BIQUADS_PEAK_MODE :
			if(Gain_dB >= 0)
			{
				norm = 1/(1 + K_div_Q + K * K)	;
				b0 = (1 + V/QValue * K + K * K) * norm	;
				b1 = 2 * (K * K - 1) * norm	;
				b2 = (1 - V/QValue * K + K * K) * norm	;
				a1 = b1	;
				a2 = (1 - K_div_Q + K * K) * norm	;
			}
			else
			{
				norm = 1/(1 + V/QValue * K + K * K)	;
				b0 = (1 + K_div_Q + K * K) * norm	;
				b1 = 2 * (K * K - 1) * norm	;
				b2 = (1 - K_div_Q + K * K) * norm	;
				a1 = b1	;
				a2 = (1 - V/QValue * K + K * K) * norm	;
			}
			break;

		case BIQUADS_LOWSHELF_MODE :
			if(Gain_dB >= 0)
			{
				norm = 1/(1 + K_div_Q + K * K)	;
				b0 = (1 + sqrt(V) * K_div_Q + V * K * K) * norm	;
				b1 = 2 * (V * K * K - 1) * norm	;
				b2 = (1 - sqrt(V) * K_div_Q + V * K * K) * norm	;
				a1 = 2 * (K * K - 1) * norm	;
				a2 = (1 - K_div_Q + K * K) * norm	;
			}
			else
			{
				norm = 1/(1 + sqrt(V) * K_div_Q + V * K * K)	;
				b0 = (1 + K_div_Q + K * K) * norm	;
				b1 = 2 * (K * K - 1) * norm	;
				b2 = (1 - K_div_Q + K * K) * norm	;
				a1 = 2 * (V * K * K - 1) * norm	;
				a2 = (1 - sqrt(V) * K_div_Q + V * K * K) * norm	;
			}
			break;

		case BIQUADS_HIGHSHELF_MODE :
			if(Gain_dB >= 0)
			{
				norm = 1/(1 + K_div_Q + K * K)	;
				b0 = (V + sqrt(V) * K_div_Q + K * K) * norm	;
				b1 = 2 * (K * K - V) * norm	;
				b2 = (V - sqrt(V) * K_div_Q + K * K) * norm	;
				a1 = 2 * (K * K - 1) * norm	;
				a2 = (1 - K_div_Q + K * K) * norm	;
			}
			else
			{
				norm = 1/(V + sqrt(V) * K_div_Q + K * K)	;
				b0 = (1 + K_div_Q + K * K) * norm	;
				b1 = 2 * (K * K - 1) * norm	;
				b2 = (1 - K_div_Q + K * K) * norm	;
				a1 = 2 * (K * K - V) * norm	;
				a2 = (V - sqrt(V) * K_div_Q + K * K) * norm	;
			}
			break;
	}


	pCoeffs[0] = b0;
	pCoeffs[1] = b1;
	pCoeffs[2] = b2;
	pCoeffs[3] = a1;
	pCoeffs[4] = a2;
}
