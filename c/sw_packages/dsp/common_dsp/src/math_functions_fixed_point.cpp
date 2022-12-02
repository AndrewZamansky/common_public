/*
 *
 * file :   math_functions_fixed_point.c
 *
 */


#include "_project_typedefs.h"
#include "_project_defines.h"

#include "common_dsp_api.h"


static real_t  one    = (int16_t)1;
static fix16_t one_fix16_t    = one;

static Fix16 fix_16    = 16.0f;
static fix16_t fix_16_fix16_t    = fix_16;

static Fix16 fix_8    = 8.0f;
static fix16_t fix_8_fix16_t    = fix_8;

// Approximation of f(x) = log2(x)
// on interval [ 8, 16 ]
// with a polynomial of degree 3.
// p(x)=((3.090795e-4*x-1.6435547e-2)*x+3.8098552e-1)*x+8.4637929e-1
//	float f(float x)
//	{
//	    float u = 3.090795e-4f;
//	    u = u * x + -1.6435547e-2f;
//	    u = u * x + 3.8098552e-1f;
//	    return u * x + 8.4637929e-1f;
//	}

static Fix16 log2_A0_8_16            = 8.4637929e-1f;
static fix16_t log2_A0_8_16_fix16_t    = log2_A0_8_16;
static Fix16 log2_A1_8_16            = 3.8098552e-1f;
static fix16_t log2_A1_8_16_fix16_t    = log2_A1_8_16;
static Fix16 log2_A2_8_16            = -1.6435547e-2f;
static fix16_t log2_A2_8_16_fix16_t    = log2_A2_8_16;
static Fix16 log2_A3_8_16            = 3.090795e-4f;
static fix16_t log2_A3_8_16_fix16_t    = log2_A3_8_16;


// Approximation of f(x) = log2(x)
// on interval [ 1, 8 ]
// with a polynomial of degree 4.
// p(x)=(((-2.367077e-3*x+5.2390269e-2)*x-4.4440251e-1)*x+1.9835619)*x-1.5720938
//	float f(float x)
//	{
//	    float u = -2.367077e-3f;
//	    u = u * x + 5.2390269e-2f;
//	    u = u * x + -4.4440251e-1f;
//	    u = u * x + 1.9835619f;
//	    return u * x + -1.5720938f;
//	}

static Fix16 log2_A0_1_8           = -1.5720938f;
static fix16_t log2_A0_1_8_fix16_t    = log2_A0_1_8;
static Fix16 log2_A1_1_8            = 1.9835619f;
static fix16_t log2_A1_1_8_fix16_t    = log2_A1_1_8;
static Fix16 log2_A2_1_8            = -4.4440251e-1f;
static fix16_t log2_A2_1_8_fix16_t    = log2_A2_1_8;
static Fix16 log2_A3_1_8            = 5.2390269e-2f;
static fix16_t log2_A3_1_8_fix16_t    = log2_A3_1_8;
static Fix16 log2_A4_1_8            = -2.367077e-3f;
static fix16_t log2_A4_1_8_fix16_t    = log2_A4_1_8;



const uint32_t tab32[32] = {
     0,  9,  1, 10, 13, 21,  2, 29,
    11, 14, 16, 18, 22, 25,  3, 30,
     8, 12, 20, 28, 15, 17, 24,  7,
    19, 27, 23,  6, 26,  5,  4, 31};

static inline fix16_t log2_32(int32_t value)
{
	int32_t ret;
	int64_t tmp_64;

	if (value > fix_16_fix16_t)
	{
		value |= value >> 1;
		value |= value >> 2;
		value |= value >> 4;
		value |= value >> 8;
		value |= value >> 16;
		ret = tab32[(uint32_t)(value * 0x07C4ACDD) >> 27];
		return  ((ret - 16) << 16);// return to fix16_t
	}
	else if (value > fix_8_fix16_t)
	{
		ret = log2_A3_8_16_fix16_t;
		tmp_64 = (int64_t)ret * value;
		ret = (tmp_64 >> 16);
		ret += log2_A2_8_16_fix16_t;
		tmp_64 = (int64_t)ret * value;
		ret = (tmp_64 >> 16);
		ret += log2_A1_8_16_fix16_t;
		tmp_64 = (int64_t)ret * value;
		ret = (tmp_64 >> 16);
		ret += log2_A0_8_16_fix16_t;
		return ret;
	}
	else
	{
		ret = log2_A4_1_8_fix16_t;
		tmp_64 = (int64_t)ret * value;
		ret = (tmp_64 >> 16);
		ret += log2_A3_1_8_fix16_t;
		tmp_64 = (int64_t)ret * value;
		ret = (tmp_64 >> 16);
		ret += log2_A2_1_8_fix16_t;
		tmp_64 = (int64_t)ret * value;
		ret = (tmp_64 >> 16);
		ret += log2_A1_1_8_fix16_t;
		tmp_64 = (int64_t)ret * value;
		ret = (tmp_64 >> 16);
		ret += log2_A0_1_8_fix16_t;
		return ret;
	}
}



// Approximation of f(x) = 2^x
// on interval [ 0, 1 ]
// with a polynomial of degree 2.
// p(x)=(3.4400111e-1*x+6.5104678e-1)*x+1.0024761
//	float f(float x)
//	{
//	    float u = 3.4400111e-1f;
//	    u = u * x + 6.5104678e-1f;
//	    return u * x + 1.0024761f;
//	}
static Fix16 exp_of_2_A0            = 1.0024761f;
static fix16_t exp_of_2_A0_fix16_t    = exp_of_2_A0;
static Fix16 exp_of_2_A1            = 0.65104678f;
static fix16_t exp_of_2_A1_fix16_t    = exp_of_2_A1;
static Fix16 exp_of_2_A2            = 0.34400111f;
static fix16_t exp_of_2_A2_fix16_t    = exp_of_2_A2;

// value is positive
static inline fix16_t exp_of_2(fix16_t value)
{
	int32_t whole_num;
	int32_t frac_num;
	fix16_t ret;
	int32_t tmp;
	int64_t tmp_64;

	whole_num = value >> 16;

	if (whole_num > 15) return 0x7fffffff;// return max of fix16
	ret = 1 << (whole_num + 16);

	frac_num = value & 0x0000ffff;

	tmp = exp_of_2_A2_fix16_t;
	tmp_64 = (int64_t)tmp * frac_num;
	tmp = (tmp_64 >> 16);
	tmp += exp_of_2_A1_fix16_t;
	tmp_64 = (int64_t)tmp * frac_num;
	tmp = (tmp_64 >> 16);
	tmp += exp_of_2_A0_fix16_t;

	tmp_64 = (int64_t)ret * tmp;
	return (tmp_64 >> 16);
}

Fix16 pow_fix_point(Fix16 a, Fix16 b)
{

	fix16_t log2_a;
	fix16_t b_log2_a;
	fix16_t result;
	int64_t tmp_64;
	fix16_t a_fix16_t;
	fix16_t b_fix16_t;

	a_fix16_t = a;
	if (a_fix16_t < one_fix16_t)
	{
		log2_a = -log2_32( fix16_div(fix16_one, a_fix16_t) );
	}
	else
	{
		log2_a = log2_32( a_fix16_t );
	}


	b_fix16_t = b;
	tmp_64 = (int64_t)log2_a * b_fix16_t;
	b_log2_a = tmp_64 >> 16;
	if (0x80000000 & b_log2_a) // negative
	{
		b_log2_a = -b_log2_a;
		result = exp_of_2(b_log2_a);
		return fix16_div(fix16_one, result);
	}
	else
	{
		return exp_of_2(b_log2_a);
	}
}
