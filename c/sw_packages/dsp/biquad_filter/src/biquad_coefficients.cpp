/*
 *
 * file :   biquad_filter.c
 *
 */



/********  includes *********************/
#include "_project_typedefs.h"
#include "_project_defines.h"

#include "dsp_management_api.h"

#include "biquad_filter_api.h" //place first to test that header file is self-contained
#include "biquad_filter.h"

#include "auto_init_api.h"

#include "math.h"

#include "_biquad_filter_prerequirements_check.h"

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
		real_t FreqC, real_t QValue, real_t Gain_dB,
		real_t SamplingRate, real_t *pCoeffs )
{
	real_t norm,V,K	;
	real_t ftem , G_abs	;
	real_t b0,b1,b2,a1,a2,w0;
	real_t K_div_Q;
	real_t one;
	real_t zero;
	real_t two;
	real_t one_minus_KdivQ_plus_KmulK;
	real_t one_div__one_plus_KdivQ_plus_KmulK;
	real_t one_plus_KdivQ_plus_KmulK;
	real_t K_pow_2_minus_one__mul_2;
	real_t K_pow_2;
	real_t tmp;
	real_t sqrt_V;
	real_t one_plus_KmulK;

	zero = (int16_t)0;
	one  = (int16_t)1;
	two  = (int16_t)2;

	b0 = 1.0;

	ftem = FreqC / SamplingRate	;
    w0 = two * ftem * (float)3.1415962;

    tmp = ftem * (float)3.1415962;
#ifdef CONFIG_DSP_REAL_NUMBER_FORMAT_FLOATING_POINT
	K = tan(tmp);
#else
	K = tmp.tan();
#endif
	G_abs = (Gain_dB >= zero) ? Gain_dB : zero - Gain_dB;

	tmp = (float)10;
#ifdef CONFIG_DSP_REAL_NUMBER_FORMAT_FLOATING_POINT
	V = pow(tmp, G_abs/(float)20);
#else
	tmp = fix16_log(tmp);
	tmp *= (G_abs / (float)20);
	V = fix16_exp(tmp);
#endif


#ifdef CONFIG_DSP_REAL_NUMBER_FORMAT_FLOATING_POINT
	sqrt_V = sqrt(V);
#else
	sqrt_V = V.sqrt();
#endif

	K_div_Q = K/QValue;

	K_pow_2 = K * K;

	one_plus_KmulK = one + K_pow_2;
	one_minus_KdivQ_plus_KmulK = one_plus_KmulK - K_div_Q;
	one_plus_KdivQ_plus_KmulK  = one_plus_KmulK + K_div_Q;
	one_div__one_plus_KdivQ_plus_KmulK = one / one_plus_KdivQ_plus_KmulK;
	K_pow_2_minus_one__mul_2 = two * (K_pow_2 - one);

	switch(filter_mode)
	{
	default :
	case BIQUADS_TRANSPARENT_MODE :
		norm = one;
		a1 = a2 = b1 = b2 = zero;
		break;

	case BIQUADS_LOWPASS_MODE_1_POLE :
		norm = one;
		tmp = zero - w0;
		#ifdef CONFIG_DSP_REAL_NUMBER_FORMAT_FLOATING_POINT
			tmp = exp(tmp);
		#else
			tmp = fix16_exp(tmp);
		#endif
		a1 = zero - tmp;
		a2 = zero;
		b0 = a1 + one;
		b1 = zero;
		b2 = zero;
		break;

	case BIQUADS_LOWPASS_MODE_2_POLES :
		norm = one_div__one_plus_KdivQ_plus_KmulK;
		b0 = K_pow_2;
		b1 = two * K_pow_2;
		b2 = K_pow_2;
		a1 = K_pow_2_minus_one__mul_2;
		a2 = one_minus_KdivQ_plus_KmulK ;
		break;

	case BIQUADS_HIGHPASS_MODE_1_POLE :
		norm = one;
		tmp = zero - w0;
		#ifdef CONFIG_DSP_REAL_NUMBER_FORMAT_FLOATING_POINT
			tmp = exp(tmp);
		#else
			tmp = fix16_exp(tmp);
		#endif
		a1 = zero - tmp;
		a2 = zero;
		b0 = (one - a1) / two;
		b1 = zero - b0;
		b2 = zero;
		break;

	case BIQUADS_HIGHPASS_MODE_2_POLES :
		norm = one_div__one_plus_KdivQ_plus_KmulK;
		b0 = one;
		b1 = zero - two;
		b2 = one;
		a1 = K_pow_2_minus_one__mul_2;
		a2 = one_minus_KdivQ_plus_KmulK;
		break;

	case BIQUADS_BANDPASS_MODE :
		norm = one_div__one_plus_KdivQ_plus_KmulK;
		b0 = K_div_Q;
		b1 = zero;
		b2 = zero - K_div_Q;
		a1 = K_pow_2_minus_one__mul_2;
		a2 = one_minus_KdivQ_plus_KmulK;
		break;

	case BIQUADS_PEAK_MODE :
		{
			real_t one_plus_VdivQmulK_plus_KmulK;
			real_t one_minus_VdivQmulK_plus_KmulK;
			real_t VdivQmulK;

			VdivQmulK = (V * K_div_Q);
			one_plus_VdivQmulK_plus_KmulK = one_plus_KmulK + VdivQmulK;
			one_minus_VdivQmulK_plus_KmulK = one_plus_KmulK - VdivQmulK;
			if(Gain_dB >= zero)
			{
				norm = one_div__one_plus_KdivQ_plus_KmulK;
				b0 = one_plus_VdivQmulK_plus_KmulK;
				b1 = K_pow_2_minus_one__mul_2;
				b2 = one_minus_VdivQmulK_plus_KmulK;
				a1 = K_pow_2_minus_one__mul_2;
				a2 = one_minus_KdivQ_plus_KmulK;
			}
			else
			{
				norm = one / one_plus_VdivQmulK_plus_KmulK;
				b0 = one_plus_KdivQ_plus_KmulK;
				b1 = K_pow_2_minus_one__mul_2;
				b2 = one_minus_KdivQ_plus_KmulK;
				a1 = K_pow_2_minus_one__mul_2;
				a2 = one_minus_VdivQmulK_plus_KmulK;
			}
		}
		break;

	case BIQUADS_LOWSHELF_MODE :
		{
			real_t one_plus_sqrtV_mul_K_div_Q_plus_V_mul_K_pow_2;
			real_t one_minus_sqrtV_mul_K_div_Q_plus_V_mul_K_pow_2;
			real_t two_mul__V_mul_K_pow_2_minus_one;
			real_t sqrtV_mul_K_div_Q;
			real_t one_plus_V_mul_K_mul_K;

			one_plus_V_mul_K_mul_K = one + V * K_pow_2;
			sqrtV_mul_K_div_Q = sqrt_V * K_div_Q;
			one_plus_sqrtV_mul_K_div_Q_plus_V_mul_K_pow_2 =
					one_plus_V_mul_K_mul_K + sqrtV_mul_K_div_Q;
			one_minus_sqrtV_mul_K_div_Q_plus_V_mul_K_pow_2 =
					one_plus_V_mul_K_mul_K - sqrtV_mul_K_div_Q;
			two_mul__V_mul_K_pow_2_minus_one = two * (V * K_pow_2 - one);

			if(Gain_dB >= zero)
			{
				norm = one_div__one_plus_KdivQ_plus_KmulK;
				b0 = one_plus_sqrtV_mul_K_div_Q_plus_V_mul_K_pow_2;
				b1 = two_mul__V_mul_K_pow_2_minus_one;
				b2 = one_minus_sqrtV_mul_K_div_Q_plus_V_mul_K_pow_2;
				a1 = K_pow_2_minus_one__mul_2;
				a2 = one_minus_KdivQ_plus_KmulK;
			}
			else
			{
				norm = one / one_plus_sqrtV_mul_K_div_Q_plus_V_mul_K_pow_2;
				b0 = one_plus_KdivQ_plus_KmulK;
				b1 = K_pow_2_minus_one__mul_2;
				b2 = one_minus_KdivQ_plus_KmulK;
				a1 = two_mul__V_mul_K_pow_2_minus_one;
				a2 = one_minus_sqrtV_mul_K_div_Q_plus_V_mul_K_pow_2;
			}
		}
		break;

	case BIQUADS_HIGHSHELF_MODE :
		{
			real_t V_plus_sqrtV_mul_K_div_Q_plus_K_pow_2;
			real_t V_minus_sqrtV_mul_K_div_Q_plus_K_pow_2;
			real_t two_mul__K_pow_2_minus_V;
			real_t sqrtV_mul_K_div_Q;
			real_t V_plus_mul_K_mul_K;

			V_plus_mul_K_mul_K = V + K_pow_2;
			sqrtV_mul_K_div_Q = sqrt_V * K_div_Q;
			V_plus_sqrtV_mul_K_div_Q_plus_K_pow_2 =
					V_plus_mul_K_mul_K + sqrtV_mul_K_div_Q;
			V_minus_sqrtV_mul_K_div_Q_plus_K_pow_2 =
					V_plus_mul_K_mul_K - sqrtV_mul_K_div_Q;
			two_mul__K_pow_2_minus_V = two * (K_pow_2 - V);

			if(Gain_dB >= zero)
			{
				norm = one_div__one_plus_KdivQ_plus_KmulK;
				b0 = V_plus_sqrtV_mul_K_div_Q_plus_K_pow_2;
				b1 = two_mul__K_pow_2_minus_V;
				b2 = V_minus_sqrtV_mul_K_div_Q_plus_K_pow_2;
				a1 = K_pow_2_minus_one__mul_2;
				a2 = one_minus_KdivQ_plus_KmulK;
			}
			else
			{
				norm = one / V_plus_sqrtV_mul_K_div_Q_plus_K_pow_2;
				b0 = one_plus_KdivQ_plus_KmulK;
				b1 = K_pow_2_minus_one__mul_2;
				b2 = one_minus_KdivQ_plus_KmulK;
				a1 = two_mul__K_pow_2_minus_V;
				a2 = V_minus_sqrtV_mul_K_div_Q_plus_K_pow_2;
			}
		}
		break;

	case BIQUADS_ALL_PASS_BUTTERWORTH_1_POLE :
		{
			real_t a;

			#ifdef CONFIG_DSP_REAL_NUMBER_FORMAT_FLOATING_POINT
				a = cos(w0) / (one + sin(w0));
			#elif defined(CONFIG_DSP_REAL_NUMBER_FORMAT_FIXED_POINT)
				a = w0.cos() / (one + w0.sin());
			#endif
			norm = one;
			a1 = zero - a;
			a2 = zero;
			b0 = zero - a;
			b1 = one;
			b2 = zero;
		}
		break;

	case BIQUADS_ALL_PASS_BUTTERWORTH_2_POLES :
		{
			real_t a;
			real_t q,p2,A,Q;
			real_t s,c;

			#ifdef CONFIG_DSP_REAL_NUMBER_FORMAT_FLOATING_POINT
				s = sin(w0);
				c = cos(w0);
			#elif defined(CONFIG_DSP_REAL_NUMBER_FORMAT_FIXED_POINT)
				s = w0.sin();
				c = w0.cos();
			#endif
			a = c / (one + s);
			A = (one + a) / (one - a);
			Q = A * (one - c) * 0.5f;

			q = (one - Q) / (one + Q);
			p2 = (one + q) * c;

			norm = one;
			a1 = zero - p2;
			a2 = q;
			b0 = q;
			b1 = zero - p2;
			b2 = one;
		}
		break;

	}


	pCoeffs[0] = b0 * norm;
	pCoeffs[1] = b1 * norm;
	pCoeffs[2] = b2 * norm;
	pCoeffs[3] = a1 * norm;
	pCoeffs[4] = a2 * norm;
//	pCoeffs[0] = 0.984415f;
//	pCoeffs[1] = -1.98417f;
//	pCoeffs[2] = 1;
//	pCoeffs[3] = 0.984415;
//	pCoeffs[4] = -1.98417f;
}
