/*
 *
 * file :   biquad_filter.c
 *
 */
#include "_project_typedefs.h"
#include "_project_defines.h"

#include "dsp_management_api.h"
#include "dsp_management_internal_api.h"

#include "biquad_filter_api.h"
#include "biquad_filter.h"

#include "auto_init_api.h"

#include "math.h"

#define B0  0
#define B1  1
#define B2  2
#define A1  3
#define A2  4


#ifdef CONFIG_DSP_REAL_NUMBER_FORMAT_FLOATING_POINT
	#define TAN(a)  tan(a)
	#define EXP(a)  exp(a)
	#define POW(a, b)  pow(a, b)
	#define SQRT(a)  sqrt(a)
	#define COS(a)  cos(a)
	#define SIN(a)  sin(a)
#else // fixed point
	#define TAN(a)  (a.tan())
	#define EXP(a)  fix16_exp(a)
	#define POW(a, b)  EXP(fix16_log(a) * b)
	#define SQRT(a)  (a.sqrt())
	#define COS(a)  (a.cos())
	#define SIN(a)  (a.sin())
#endif

static real_t __one = (int16_t)1;
static real_t __zero = (int16_t)0;
static real_t __two = (int16_t)2;
static real_t PI = (float)3.1415926535897932f;
static real_t LN_10_div_20 = (float)0.1151292546497f;


static void apply_normalization(real_t *pCoeffs, real_t norm)
{
	for (uint8_t i = 0; i < 5; i++) pCoeffs[i] *= norm;
}


static void calc_transparent_mode(real_t *pCoeffs)
{
	pCoeffs[B0] = __one;
	pCoeffs[B1] = __zero;
	pCoeffs[B2] = __zero;
	pCoeffs[A1] = __zero;
	pCoeffs[A2] = __zero;
}


static void calc_lowpass_mode_1_pole(real_t *pCoeffs, real_t w0)
{
	real_t tmp;
	real_t a1;

	tmp = EXP(__zero - w0);
	a1 = __zero - tmp;
	pCoeffs[A1] = a1;
	pCoeffs[A2] = __zero;
	pCoeffs[B0] =  a1 + __one;
	pCoeffs[B1] = __zero;
	pCoeffs[B2] = __zero;
}


static void calc_lowpass_mode_2_poles(
		real_t *pCoeffs, real_t K_pow_2, real_t K_div_Q)
{
	real_t norm;

	pCoeffs[B0] =  K_pow_2;
	pCoeffs[B1] = __two * K_pow_2;
	pCoeffs[B2] = K_pow_2;
	pCoeffs[A1] = __two * (K_pow_2 - __one);
	pCoeffs[A2] = __one + K_pow_2 - K_div_Q;
	norm = __one / (__one + K_pow_2 + K_div_Q);
	apply_normalization(pCoeffs, norm);
}


static void calc_highpass_mode_1_pole(real_t *pCoeffs, real_t w0)
{
	real_t tmp;
	real_t a1;
	real_t b0;

	tmp = EXP(__zero - w0);
	a1 = __zero - tmp;
	b0 = (__one - a1) / __two;
	pCoeffs[A1] = a1;
	pCoeffs[A2] = __zero;
	pCoeffs[B0] =  b0;
	pCoeffs[B1] = __zero - b0;
	pCoeffs[B2] = __zero;
}


static void calc_highpass_mode_2_poles(
		real_t *pCoeffs, real_t K_pow_2, real_t K_div_Q)
{
	real_t norm;

	pCoeffs[B0] =  __one;
	pCoeffs[B1] = __zero - __two;
	pCoeffs[B2] = __one;
	pCoeffs[A1] = __two * (K_pow_2 - __one);
	pCoeffs[A2] = __one + K_pow_2 - K_div_Q;
	norm = __one / (__one + K_pow_2 + K_div_Q);
	apply_normalization(pCoeffs, norm);
}


static void calc_bandpass_mode(real_t *pCoeffs, real_t K_pow_2, real_t K_div_Q)
{
	real_t norm;

	pCoeffs[B0] =  K_div_Q;
	pCoeffs[B1] = __zero;
	pCoeffs[B2] = __zero - K_div_Q;
	pCoeffs[A1] = __two * (K_pow_2 - __one);
	pCoeffs[A2] = __one + K_pow_2 - K_div_Q;
	norm = __one / (__one + K_pow_2 + K_div_Q);
	apply_normalization(pCoeffs, norm);
}


static void calc_peak_mode(real_t *pCoeffs,
		real_t V, real_t K_pow_2, real_t K_div_Q, real_t Gain_dB)
{
	real_t norm;
	real_t one_plus_VdivQmulK_plus_KmulK;
	real_t one_minus_VdivQmulK_plus_KmulK;
	real_t K_pow_2_minus_one__mul_2;
	real_t one_minus_KdivQ_plus_KmulK;
	real_t one_plus_KdivQ_plus_KmulK;
	real_t VdivQmulK;
	real_t one_plus_KmulK;

	one_plus_KmulK = __one + K_pow_2;
	VdivQmulK = (V * K_div_Q);
	one_plus_VdivQmulK_plus_KmulK = one_plus_KmulK + VdivQmulK;
	one_minus_VdivQmulK_plus_KmulK = one_plus_KmulK - VdivQmulK;
	K_pow_2_minus_one__mul_2 = __two * (K_pow_2 - __one);
	one_minus_KdivQ_plus_KmulK = one_plus_KmulK - K_div_Q;
	one_plus_KdivQ_plus_KmulK  = one_plus_KmulK + K_div_Q;
	if(Gain_dB >= __zero)
	{
		norm = __one / one_plus_KdivQ_plus_KmulK;
		pCoeffs[B0] = one_plus_VdivQmulK_plus_KmulK;
		pCoeffs[B1] = K_pow_2_minus_one__mul_2;
		pCoeffs[B2] = one_minus_VdivQmulK_plus_KmulK;
		pCoeffs[A1] = K_pow_2_minus_one__mul_2;
		pCoeffs[A2] = one_minus_KdivQ_plus_KmulK;
	}
	else
	{
		norm = __one / one_plus_VdivQmulK_plus_KmulK;
		pCoeffs[B0] = one_plus_KdivQ_plus_KmulK;
		pCoeffs[B1] = K_pow_2_minus_one__mul_2;
		pCoeffs[B2] = one_minus_KdivQ_plus_KmulK;
		pCoeffs[A1] = K_pow_2_minus_one__mul_2;
		pCoeffs[A2] = one_minus_VdivQmulK_plus_KmulK;
	}
	apply_normalization(pCoeffs, norm);
}


static void calc_lowshelf_mode(real_t *pCoeffs,
		real_t V, real_t K_pow_2, real_t K_div_Q, real_t Gain_dB)
{
	real_t norm;
	real_t one_plus_sqrtV_mul_K_div_Q_plus_V_mul_K_pow_2;
	real_t one_minus_sqrtV_mul_K_div_Q_plus_V_mul_K_pow_2;
	real_t two_mul__V_mul_K_pow_2_minus_one;
	real_t sqrtV_mul_K_div_Q;
	real_t one_plus_V_mul_K_mul_K;
	real_t one_plus_KdivQ_plus_KmulK;
	real_t one_minus_KdivQ_plus_KmulK;
	real_t one_plus_KmulK;
	real_t K_pow_2_minus_one__mul_2;

	one_plus_KmulK = __one + K_pow_2;
	one_plus_V_mul_K_mul_K = __one + V * K_pow_2;
	sqrtV_mul_K_div_Q = SQRT(V) * K_div_Q;
	one_plus_sqrtV_mul_K_div_Q_plus_V_mul_K_pow_2 =
			one_plus_V_mul_K_mul_K + sqrtV_mul_K_div_Q;
	one_minus_sqrtV_mul_K_div_Q_plus_V_mul_K_pow_2 =
			one_plus_V_mul_K_mul_K - sqrtV_mul_K_div_Q;
	two_mul__V_mul_K_pow_2_minus_one = __two * (V * K_pow_2 - __one);
	one_minus_KdivQ_plus_KmulK = one_plus_KmulK - K_div_Q;
	one_plus_KdivQ_plus_KmulK  = one_plus_KmulK + K_div_Q;
	K_pow_2_minus_one__mul_2 = __two * (K_pow_2 - __one);

	if(Gain_dB >= __zero)
	{
		norm = __one / one_plus_KdivQ_plus_KmulK;
		pCoeffs[B0] = one_plus_sqrtV_mul_K_div_Q_plus_V_mul_K_pow_2;
		pCoeffs[B1] = two_mul__V_mul_K_pow_2_minus_one;
		pCoeffs[B2] = one_minus_sqrtV_mul_K_div_Q_plus_V_mul_K_pow_2;
		pCoeffs[A1] = K_pow_2_minus_one__mul_2;
		pCoeffs[A2] = one_minus_KdivQ_plus_KmulK;
	}
	else
	{
		norm = __one / one_plus_sqrtV_mul_K_div_Q_plus_V_mul_K_pow_2;
		pCoeffs[B0] = one_plus_KdivQ_plus_KmulK;
		pCoeffs[B1] = K_pow_2_minus_one__mul_2;
		pCoeffs[B2] = one_minus_KdivQ_plus_KmulK;
		pCoeffs[A1] = two_mul__V_mul_K_pow_2_minus_one;
		pCoeffs[A2] = one_minus_sqrtV_mul_K_div_Q_plus_V_mul_K_pow_2;
	}
	apply_normalization(pCoeffs, norm);
}


static void calc_highshelf_mode(real_t *pCoeffs,
		real_t V, real_t K_pow_2, real_t K_div_Q, real_t Gain_dB)
{
	real_t norm;
	real_t V_plus_sqrtV_mul_K_div_Q_plus_K_pow_2;
	real_t V_minus_sqrtV_mul_K_div_Q_plus_K_pow_2;
	real_t two_mul__K_pow_2_minus_V;
	real_t sqrtV_mul_K_div_Q;
	real_t V_plus_mul_K_mul_K;
	real_t one_plus_KdivQ_plus_KmulK;
	real_t one_minus_KdivQ_plus_KmulK;
	real_t one_plus_KmulK;
	real_t K_pow_2_minus_one__mul_2;

	one_plus_KmulK = __one + K_pow_2;
	V_plus_mul_K_mul_K = V + K_pow_2;
	sqrtV_mul_K_div_Q = SQRT(V) * K_div_Q;
	V_plus_sqrtV_mul_K_div_Q_plus_K_pow_2 =
			V_plus_mul_K_mul_K + sqrtV_mul_K_div_Q;
	V_minus_sqrtV_mul_K_div_Q_plus_K_pow_2 =
			V_plus_mul_K_mul_K - sqrtV_mul_K_div_Q;
	two_mul__K_pow_2_minus_V = __two * (K_pow_2 - V);
	one_minus_KdivQ_plus_KmulK = one_plus_KmulK - K_div_Q;
	one_plus_KdivQ_plus_KmulK  = one_plus_KmulK + K_div_Q;
	K_pow_2_minus_one__mul_2 = __two * (K_pow_2 - __one);

	if(Gain_dB >= __zero)
	{
		norm = __one / one_plus_KdivQ_plus_KmulK;
		pCoeffs[B0] = V_plus_sqrtV_mul_K_div_Q_plus_K_pow_2;
		pCoeffs[B1] = two_mul__K_pow_2_minus_V;
		pCoeffs[B2] = V_minus_sqrtV_mul_K_div_Q_plus_K_pow_2;
		pCoeffs[A1] = K_pow_2_minus_one__mul_2;
		pCoeffs[A2] = one_minus_KdivQ_plus_KmulK;
	}
	else
	{
		norm = __one / V_plus_sqrtV_mul_K_div_Q_plus_K_pow_2;
		pCoeffs[B0] = one_plus_KdivQ_plus_KmulK;
		pCoeffs[B1] = K_pow_2_minus_one__mul_2;
		pCoeffs[B2] = one_minus_KdivQ_plus_KmulK;
		pCoeffs[A1] = two_mul__K_pow_2_minus_V;
		pCoeffs[A2] = V_minus_sqrtV_mul_K_div_Q_plus_K_pow_2;
	}
	apply_normalization(pCoeffs, norm);
}


static void calc_allpass_butterworth_mode_1_pole(real_t *pCoeffs, real_t w0)
{
	real_t a;

	a = COS(w0) / (__one + SIN(w0));
	pCoeffs[A1] = __zero - a;
	pCoeffs[A2] = __zero;
	pCoeffs[B0] = __zero - a;
	pCoeffs[B1] = __one;
	pCoeffs[B2] = __zero;
}


static void calc_allpass_butterworth_mode_2_poles(real_t *pCoeffs, real_t w0)
{
	real_t a;
	real_t q, p2, A, Q;
	real_t c;

	c = COS(w0);
	a = c / (__one + SIN(w0));
	A = (__one + a) / (__one - a);
	Q = A * (__one - c) * 0.5f;

	q = (__one - Q) / (__one + Q);
	p2 = (__one + q) * c;

	pCoeffs[A1] = __zero - p2;
	pCoeffs[A2] = q;
	pCoeffs[B0] = q;
	pCoeffs[B1] = __zero - p2;
	pCoeffs[B2] = __one;
}


/*   func : biquads_coefficients_calculation_common()
 *    The coefficients are stored in the array pCoeffs in the following order:
 *
 *     y[n] = b0 * x[n] + b1 * x[n-1] + b2 * x[n-2] + a1 * y[n-1] + a2 * y[n-2]
 *
 *    {b10, b11, b12, a11, a12, b20, b21, b22, a21, a22, ...}
 */
void biquads_coefficients_calculation_common(
		enum biquads_filter_mode_e filter_mode,
		real_t FreqC, real_t QValue, real_t Gain_dB,
		real_t SamplingRate, real_t *pCoeffs )
{
	real_t V;
	real_t K;
	real_t ftem;
	real_t G_abs;
	real_t w0;
	real_t K_div_Q;
	real_t K_pow_2;

	ftem = FreqC / SamplingRate	;
	w0 = __two * ftem * PI;

	K = TAN(ftem * PI);
	G_abs = (Gain_dB >= __zero) ? Gain_dB : (__zero - Gain_dB);
	V = EXP(LN_10_div_20 * G_abs);// = pow(10, G_abs/20);

	K_div_Q = K / QValue;
	K_pow_2 = K * K;

	switch(filter_mode)
	{
	default :
	case BIQUADS_TRANSPARENT_MODE:
		calc_transparent_mode(pCoeffs);
		break;
	case BIQUADS_LOWPASS_MODE_1_POLE:
		calc_lowpass_mode_1_pole(pCoeffs, w0);
		break;
	case BIQUADS_LOWPASS_MODE_2_POLES:
		calc_lowpass_mode_2_poles(pCoeffs, K_pow_2, K_div_Q);
		break;
	case BIQUADS_HIGHPASS_MODE_1_POLE:
		calc_highpass_mode_1_pole(pCoeffs, w0);
		break;
	case BIQUADS_HIGHPASS_MODE_2_POLES:
		calc_highpass_mode_2_poles(pCoeffs, K_pow_2, K_div_Q);
		break;
	case BIQUADS_BANDPASS_MODE:
		calc_bandpass_mode(pCoeffs, K_pow_2, K_div_Q);
		break;
	case BIQUADS_PEAK_MODE:
		calc_peak_mode(pCoeffs, V, K_pow_2, K_div_Q, Gain_dB);
		break;
	case BIQUADS_LOWSHELF_MODE:
		calc_lowshelf_mode(pCoeffs, V, K_pow_2, K_div_Q, Gain_dB);
		break;
	case BIQUADS_HIGHSHELF_MODE:
		calc_highshelf_mode(pCoeffs, V, K_pow_2, K_div_Q, Gain_dB);
		break;
	case BIQUADS_ALL_PASS_BUTTERWORTH_1_POLE:
		calc_allpass_butterworth_mode_1_pole(pCoeffs, w0);
		break;
	case BIQUADS_ALL_PASS_BUTTERWORTH_2_POLES :
		calc_allpass_butterworth_mode_2_poles(pCoeffs, w0);
		break;
	}
}
