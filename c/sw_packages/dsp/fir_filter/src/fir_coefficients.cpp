/*
 *
 * file :   fir_filter.c
 *
 */
#include "_project_typedefs.h"
#include "_project_defines.h"

#include "dsp_management_api.h"
#include "dsp_management_internal_api.h"

#include "fir_filter_api.h"
#include "fir_filter.h"

#include "math.h"


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
static real_t PIH = (float)1.57079632679490f;
static real_t errorlimit = (float)1e-9;
static real_t const_20_96 = (float)20.96f;
static real_t const_50_0 = (float)50.0f;
static real_t const_0_1102 = (float)0.1102f;
static real_t const_8_71 = (float)8.71f;
static real_t const_0_5842 = (float)0.5842f;
static real_t const_0_4 = (float)0.4f;
static real_t const_0_07886 = (float)0.07886f;


static void calc_transparent_mode(real_t *p_coeffs, int16_t coeff_num)
{
	p_coeffs[coeff_num - 1] = __one;
	for (int16_t i = 0; i < coeff_num; i++) p_coeffs[0] = __zero;
}


static real_t Izero (real_t x)
{
	real_t x2 = x / __two;
	real_t summe = __one;
	real_t ds =  __one;
	real_t di =  __one;
	real_t tmp;
	do
	{
		tmp = x2 / di;
		tmp *= tmp;
		ds *= tmp;
		summe += ds;
		di += __one;
	} while (ds >= errorlimit * summe);
	return (summe);
}


static void calc_hilbert_transform_mode(
		real_t *p_coeffs, int16_t coeff_num, real_t Beta)
{
	int16_t i;
	int16_t nc;
	real_t x;
	real_t w;
	real_t izb;

	izb = Izero(Beta);
	nc =  2 * (coeff_num / 2);

	// set real delay
	p_coeffs[nc] = __one;

	// set imaginary Hilbert coefficients
	for (i = 1; i < (nc + 1); i += 2)
	{
		if ((2 * i) == nc) continue;
		x = ((__two * i) - nc) / nc;
		w = Izero(Beta * SQRT(__one - x * x)) / izb; // Kaiser window
		p_coeffs[(2 * i) + 1] = __one / (PIH * (i - (nc / 2))) * w;
	}
}


static real_t m_sinc(int16_t m, real_t fc)
{
	// fc is f_cut/(Fsamp/2)
	// m is between -M and M step 2
	real_t x = PIH * m;
	if (m == 0)
	{
		return __one;
	}
	else
	{
		return SIN(x * fc) / (fc * x);
	}
}


static void calc_highpass_transform_mode(
		real_t *p_coeffs, int16_t coeff_num, real_t f_norm, real_t Beta)
{
	int16_t i, j;
	int16_t nc;
	real_t izb;
	real_t w;

	f_norm *= 2;
	nc = (int16_t)( 2 * (coeff_num / 2));
	izb = Izero (Beta);
	for (i = - nc, j = 0; i <= nc; i += 2, j++)
	{
		real_t x = i;
		x /= nc;
		w = Izero(Beta * SQRT(__one - x * x)) / izb; // Kaiser window

		// populate in reverse order
		p_coeffs[nc - j] = (m_sinc(i, __one) - f_norm * m_sinc(i, f_norm)) * w;
	}
}


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
		real_t sample_rate, real_t *p_coeffs, size_t coeff_num)
{
	int16_t i, j;
	real_t Beta;
	real_t izb;
	real_t fcf = fc;
	int16_t nc;

	if (coeff_num > 0x7fff)
	{
		CRITICAL_ERROR("don't support very long filters");
	}

	if ( 0 == (coeff_num % 2))
	{
		CRITICAL_ERROR("support only odd num of taps");
	}

	// clear all coefficients
	for (i = 0; i < coeff_num; i++) p_coeffs[i] = __zero;

	nc = (int16_t)coeff_num;
	fc = fc / sample_rate;
	dfc = dfc / sample_rate;
	// calculate Kaiser-Bessel window shape factor beta
	// from stop-band attenuation
	if (Astop < const_20_96)
	{
		Beta = __zero;
	}
	else if (Astop >= const_50_0)
	{
		Beta = const_0_1102 * (Astop - const_8_71);
	}
	else
	{
		Beta = (const_0_5842 * POW((Astop - const_20_96), const_0_4)) +
									const_0_07886 * (Astop - const_20_96);
	}

	switch (filter_mode)
	{
	default:
	case FIR_TRANSPARENT_MODE:
		calc_transparent_mode(p_coeffs, (int16_t)coeff_num);
		return;
	case FIR_LOWPASS_MODE:
		fcf = fc * __two;
		nc =  (int16_t)(2 * (coeff_num / 2));
		break;
	case FIR_HIGHPASS_MODE:
		calc_highpass_transform_mode(p_coeffs, coeff_num, fc, Beta);
		return;
	case FIR_BANDPASS_MODE:
	case FIR_NOTCH_MODE:
		fc = fc * __two;
		fcf = dfc;
		nc =  (int16_t)(2 * (coeff_num / 2));
		break;
	case FIR_HILBERT_TRANSFORM_MODE:
		calc_hilbert_transform_mode(p_coeffs, (int16_t)coeff_num, Beta);
		return;
	}


	izb = Izero (Beta);
	for (i = - nc, j = 0; i <= nc; i += 2, j++)
	{
		real_t x = i;
		x /= nc;
		real_t w = Izero(Beta * SQRT(__one - x * x)) / izb; // Kaiser window

		// populate in reverse order
		p_coeffs[nc - j] = fcf * m_sinc(i, fcf) * w;
	}

	switch (filter_mode)
	{
	case FIR_BANDPASS_MODE:
		for (j = 0; j < (nc + 1); j++)
		{
			p_coeffs[nc - j] *= __two * COS(PIH * ((__two * j) - nc) * fc);
		}
		break;
	case FIR_NOTCH_MODE:
		for (j = 0; j < (nc + 1); j++)
		{
			p_coeffs[nc - j] *=
					__zero - __two * COS(PIH * ((__two * j) - nc) * fc);
		}
		p_coeffs[nc / 2] += __one;
		break;
	default:
		break;
	}
}
