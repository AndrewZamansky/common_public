/*
 *
 * file :   math_functions.c
 *
 */


#include "_project_typedefs.h"
#include "_project_defines.h"

#include "common_dsp_api.h"
#include <math.h>

#if defined(__GNUC__)
	#if (4 != __SIZEOF_FLOAT__)
		#error "only floats of 4 byte size are supported"
	#endif
	#if ((__BYTE_ORDER__ != __ORDER_LITTLE_ENDIAN__) || \
			(__FLOAT_WORD_ORDER__ != __ORDER_LITTLE_ENDIAN__))
		#error "only little endian order supported supported"
	#endif
#else
	#warning "not tested yet"
#endif

#define USE_LOG2_EIGHTH_ORDER
#ifdef USE_LOG2_EIGHTH_ORDER
	#define A0_log2  (-0.41503749927884381855f)
	#define A1_log2  (1.92359338785195120981f)
	#define A2_log2  (-1.28239559190130080654f)
	#define A3_log2  (1.13990719280115627248f)
	#define A4_log2  (-1.13990719280115627248f)
	#define A5_log2  (1.21590100565456669065f)
	#define A6_log2  (-1.35100111739396298961f)
	#define A7_log2  (1.54400127702167198812f)
	#define A8_log2  (-1.80133482319195065281f)
#else
	#define A2_log2  (-1.28239559190130080654f)
	#define A1_log2  (3.84718677570390241963f)
	#define A0_log2  (-2.57908006061228892959f)
#endif

static inline float b_mult_log2_of_a(float b, float a)
{
	float     fraction;
	int       exp;
	uint32_t  a_in_raw_bit_represantation;
	uint32_t  *tmp_casting_pointer_32;
	float     *tmp_casting_float;
	uint32_t  fraction_in_bit_represantation;
	float     log2_of_fraction ;
	float     log2_of_a;

	tmp_casting_pointer_32 = (uint32_t*)&a;
	a_in_raw_bit_represantation = *tmp_casting_pointer_32;
	exp = a_in_raw_bit_represantation >> 23;

	/* we want to calculate b * ( log2((1 + fraction)/2) * (exp + 1) )  */

	/* calculation of (1 + fraction)/2  (by writing 0x7e into exponent part)*/
	a_in_raw_bit_represantation = a_in_raw_bit_represantation & 0x7fffff;
	fraction_in_bit_represantation =
			a_in_raw_bit_represantation | ( (-1 + 127)<<23 );

	tmp_casting_float = (float*)&fraction_in_bit_represantation;
	fraction = *tmp_casting_float;

#ifdef USE_LOG2_EIGHTH_ORDER
	/* log2(a) = exp + log2(fraction)
	 * fraction = [0.5,1] so it can be
	 * approximated by taylor series around 0.75 :
	 * log2(fraction) ~= sum( An_log2 * (fraction-0.75)^n )
	 */
	fraction -= 0.75f;

	log2_of_fraction = A8_log2;
	log2_of_fraction *= fraction;
	log2_of_fraction += A7_log2;
	log2_of_fraction *= fraction;
	log2_of_fraction += A6_log2;
	log2_of_fraction *= fraction;
	log2_of_fraction += A5_log2;
	log2_of_fraction *= fraction;
	log2_of_fraction += A4_log2;
	log2_of_fraction *= fraction;
	log2_of_fraction += A3_log2;
	log2_of_fraction *= fraction;
	log2_of_fraction += A2_log2;
	log2_of_fraction *= fraction;
	log2_of_fraction += A1_log2;
	log2_of_fraction *= fraction;
	log2_of_fraction +=  A0_log2 ;
#else
	/* log2(a) = exp + log2(fraction)
	 * fraction = [0.5,1] so it can be
	 * approximated by taylor series around 0.75 :
	 * log2(fraction) ~= A2_log2 * fraction^2 + A2_log1 * fraction  + A2_log0
	 */
	log2_of_fraction = fraction * A2_log2;
	log2_of_fraction += A1_log2;
	log2_of_fraction *= fraction;
	log2_of_fraction +=  A0_log2 ;
#endif

	log2_of_a = log2_of_fraction + (exp - 127 + 1)  ;

	return ( b * log2_of_a);
}

//#define USE_TAYLOR_AROUND_0_5  //for taylor around x=0.5 (not for neg numbers)
#define USE_SEVENTH_TAYLOR_POWER
//#define USE_THIRD_TAYLOR_POWER
#ifdef USE_THIRD_TAYLOR_POWER
	#define A3_2_power_x  (0.05550410866482157995f)
	#define A2_2_power_x  (0.24022650695910071233f)
	#define A1_2_power_x  (0.69314718055994530942f)
	#define A0_2_power_x  (1.0f)
#elif defined(USE_SEVENTH_TAYLOR_POWER)
	#ifdef USE_TAYLOR_AROUND_0_5 //for taylor around x=0.5
		#define A7_2_power_x  (0.000021570623008968f)
		#define A6_2_power_x  (0.00021783881590746449f)
		#define A5_2_power_x  (0.00188564987653693711f)
		#define A4_2_power_x  (0.01360208862866362641f)
		#define A3_2_power_x  (0.07849466324122069881f)
		#define A2_2_power_x  (0.33973158418307492704f)
		#define A1_2_power_x  (0.98025814346854719171f)
		#define A0_2_power_x  (1.4142135623730950488f)
	#else
		#define A7_2_power_x  (0.00001525273380405984f)
		#define A6_2_power_x  (0.0001540353039338161f)
		#define A5_2_power_x  (0.00133335581464284434f)
		#define A4_2_power_x  (0.00961812910762847716f)
		#define A3_2_power_x  (0.05550410866482157995f)
		#define A2_2_power_x  (0.24022650695910071233f)
		#define A1_2_power_x  (0.69314718055994530942f)
		#define A0_2_power_x  (1.0f)
	#endif
#else
	#define A2_2_power_x  (0.33973f)
	#define A1_2_power_x  (0.64053f)
	#define A0_2_power_x  (1.009f)
#endif
/* function : fast_pow
 *	assuming that a is positive
 *	floating number IEEE754
 *	U32=[sign][exp][fraction] -> Fl32 = ((-1)^sign)*(1 + fraction)*2^(exp - 127)
 *	y=a^b=2^z when z=b*log2(a)
 *
 *	1) first we calculating z :
 *	z = b*(log2(1 + fraction) + exp - 127) =
 *	= b*(log2((1 + fraction)/2) + (exp - 127 + 1))
 *  we calculate log2((1 + fraction)/2) instead log2(1 + fraction)) as first
 *  expression can be better approximated by taylor series
 *  2) represent z as int_z + fraction_z
 *  3) calculate 2^int_z by shift
 *  4) calculate 2^fraction_z  by taylor
 *  5) result = 2^int_z  *  2^fraction_z
 */
float fast_pow_float(float a, float b)
{
	float    z;
	int32_t  int_of_z;
	float    fraction_of_z;
	float    ret_val;
	float    power_of_fraction;
	uint32_t  *tmp_casting_pointer_32;

	if (a < 0)
	{
		CRITICAL_ERROR("a must be positive");
	}
	z = b_mult_log2_of_a(b, a);


	//2^z calculation
	int_of_z = (int32_t)z ;

	tmp_casting_pointer_32 = (uint32_t*)&ret_val;

	// note: int_of_z may be negative

	//calculation of integer part:
	*tmp_casting_pointer_32 = (int_of_z + 127) << 23;
	fraction_of_z = z - (float)int_of_z;

#ifdef USE_THIRD_TAYLOR_POWER
	/* fraction_of_z = [0.5,1] so it can be
	 * approximated by taylor series around 0  :
	 * 2^fraction_of_z ~= A3_2_power_x * fraction_of_z^3 +
	 *                    A2_2_power_x * fraction_of_z^2 +
	 *                     A1_2_power_x * fraction_of_z  + A0_2_power_x
	 */
	power_of_fraction = fraction_of_z * A3_2_power_x;
	power_of_fraction += A2_2_power_x;
	power_of_fraction *= fraction_of_z ;
	power_of_fraction += A1_2_power_x;
	power_of_fraction *= fraction_of_z ;
	power_of_fraction += A0_2_power_x;
#elif defined(USE_SEVENTH_TAYLOR_POWER)
	/* fraction_of_z = [0,1] so it can be
	 * approximated by taylor series around 0.5  :
	 *  2^fraction_of_z ~= An_2_power_x * (fraction_of_z - 0.5)^n
	 */
#ifdef USE_TAYLOR_AROUND_0_5
	fraction_of_z -= 0.5f;
#endif
	power_of_fraction = 0.0f;
	power_of_fraction += A7_2_power_x;
	power_of_fraction *= fraction_of_z ;
	power_of_fraction += A6_2_power_x;
	power_of_fraction *= fraction_of_z ;
	power_of_fraction += A5_2_power_x;
	power_of_fraction *= fraction_of_z ;
	power_of_fraction += A4_2_power_x;
	power_of_fraction *= fraction_of_z ;
	power_of_fraction += A3_2_power_x;
	power_of_fraction *= fraction_of_z ;
	power_of_fraction += A2_2_power_x;
	power_of_fraction *= fraction_of_z ;
	power_of_fraction += A1_2_power_x;
	power_of_fraction *= fraction_of_z ;
	power_of_fraction += A0_2_power_x;
#else
	/* fraction_of_z = [0.5,1] so it can be
	 * approximated by taylor series around 0  :
	 *  2^fraction_of_z ~= A3_2_power_x * fraction_of_z^3 +
	 *                     A2_2_power_x * fraction_of_z^2 +
	 *                     A1_2_power_x * fraction_of_z  + A0_2_power_x
	 */
	tmp = fraction_of_z * fraction_of_z;
	power_of_fraction =  A2_2_power_x * tmp;
	tmp = A1_2_power_x * fraction_of_z;
	power_of_fraction += tmp;
	power_of_fraction += A0_2_power_x;
#endif

	ret_val = ret_val * power_of_fraction;

	return ret_val;
}


static float log2f_approx_coeff[4] = {1.23149591368684f,
		-4.11852516267426f, 6.02197014179219f, -3.13396450166353f};
/*
 * Fast approximation to the log2() function.  It uses a two step
 * process.  First, it decomposes the floating-point number into
 * a fractional component F and an exponent E.  The fraction component
 * is used in a polynomial approximation and then the exponent added
 * to the result.  A 3rd order polynomial is used and the result
 * when computing db20() is accurate to 7.984884e-003 dB.
 */
float log2f_approx_float(float X)
{
  float *C = &log2f_approx_coeff[0];
  float Y;
  float F;
  int E;

  // This is the approximation to log2()
  F = frexpf(fabsf(X), &E);

  //  Y = C[0]*F*F*F + C[1]*F*F + C[2]*F + C[3] + E;

  Y = *C++;
  Y *= F;
  Y += (*C++);
  Y *= F;
  Y += (*C++);
  Y *= F;
  Y += (*C++);
  Y += E;

  return(Y);
}
