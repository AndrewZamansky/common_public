/*
 *
 * file :   math_functions.c
 *
 *
 *
 *
 *
 */



/********  includes *********************/

#include "common_dsp_config.h"
//#include "dev_managment_api.h" // for device manager defines and typedefs
//#include "dsp_managment_api.h" // for device manager defines and typedefs
#include "_common_dsp_prerequirements_check.h" // should be after {equalizer_config.h,dev_managment_api.h}


#include "common_dsp_api.h"

#include <math.h>

/********  defines *********************/


/********  types  *********************/

/********  externals *********************/


/********  local defs *********************/


/**********   external variables    **************/



/***********   local variables    **************/

#define A2_log2		(-1.2823956f)
#define A1_log2		(3.8471867f)
#define A0_log2		(-2.57908006f)

//#define USE_THIRD_TALOR_POWER
#ifdef USE_THIRD_TALOR_POWER
	#define A3_2_power_x		(0.055504f)
	#define A2_2_power_x		(0.24023f)
	#define A1_2_power_x		(0.69315f)
	#define A0_2_power_x		(1.0f)
#else
	#define A2_2_power_x		(0.33973f)
	#define A1_2_power_x		(0.64053f)
	#define A0_2_power_x		(1.009f)
#endif
/* function : fast_pow
 *	y=a^b=2^z when z=b*log2(a)
 *	assuming that a is positive
 */
float fast_pow(float a, float b)
{
	int exp;
	float	fraction;
	float	log2_of_fraction;
	float	tmp;
	float	log2_of_a;
	float	z;
	uint16_t	int_of_z;
	float	fraction_of_z;
	float retVal;
	float	power_of_fraction;
	uint8_t negative_power_sign ;
	uint32_t	a_in_raw_bit_represantation;
	uint64_t power_of_2_of_integer_part;
	a_in_raw_bit_represantation=*((uint32_t*)&a);
	//log2(a) calculation
	//fraction = frexpf(a, &exp);
	exp = a_in_raw_bit_represantation >> 23;
	exp = exp - 127 + 1; // +1 to have fraction = mantissa/2
	a_in_raw_bit_represantation = a_in_raw_bit_represantation & 0x7fffff;
	a_in_raw_bit_represantation = a_in_raw_bit_represantation | ((-1+127)<<23);// -1 to have fraction = mantissa/2
	*(uint32_t*)&fraction = a_in_raw_bit_represantation;

		// log2(a) = exp + log2(fraction)
	    // fraction = [0.5,1] so it can be approximated by taylor series around 0.75 :
		// log2(fraction) ~= A2_log2 * fraction^2 + A2_log1 * fraction  + A2_log0
	tmp =  fraction * fraction;
	log2_of_fraction = tmp * A2_log2;
	tmp = A1_log2 * fraction;
	log2_of_fraction +=  tmp ;
	log2_of_fraction +=  A0_log2 ;

	log2_of_a = log2_of_fraction + exp  ;
	//end of log2(a) calculation




	z = b * log2_of_a;
	if(0 > z)
	{
		negative_power_sign = 1;
		z = -z;
	}
	else
	{
		negative_power_sign =0;
	}

	//2^z calculation
	int_of_z = (uint16_t)z ;
//	power_of_2_of_integer_part = (((uint64_t)1)<<int_of_z);
//	retVal = (float)power_of_2_of_integer_part;
	*(uint32_t*)&retVal = (int_of_z+127)<<23;

	fraction_of_z = z - int_of_z;
#ifdef USE_THIRD_TALOR_POWER
		// fraction_of_z = [0.5,1] so it can be approximated by taylor series around 0  :
		// 2^fraction_of_z ~= A3_2_power_x * fraction_of_z^3 + A2_2_power_x * fraction_of_z^2 + A1_2_power_x * fraction_of_z  + A0_2_power_x
	power_of_fraction =  A3_2_power_x * fraction_of_z;
	tmp = fraction_of_z * fraction_of_z;
	power_of_fraction =  power_of_fraction * tmp;
	tmp = tmp * A2_2_power_x;
	power_of_fraction += tmp;
	tmp = A1_2_power_x * fraction_of_z ;
	power_of_fraction += tmp;
	power_of_fraction += A0_2_power_x;
#else
	// fraction_of_z = [0.5,1] so it can be approximated by taylor series around 0  :
	// 2^fraction_of_z ~= A3_2_power_x * fraction_of_z^3 + A2_2_power_x * fraction_of_z^2 + A1_2_power_x * fraction_of_z  + A0_2_power_x
	tmp = fraction_of_z * fraction_of_z;
	power_of_fraction =  A2_2_power_x * tmp;
	tmp = A1_2_power_x * fraction_of_z;
	power_of_fraction += tmp;
	power_of_fraction += A0_2_power_x;
#endif
	retVal = retVal * power_of_fraction;
	//2^z calculation

	if(1 == negative_power_sign)
	{
		retVal = 1/retVal;
	}

	return retVal;
}
