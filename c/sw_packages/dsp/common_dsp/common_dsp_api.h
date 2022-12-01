#ifndef _common_dsp_API_H_
#define _common_dsp_API_H_

float fast_pow_float(float a, float b);
float log2f_approx_float(float X);

#if defined(CONFIG_DSP_REAL_NUMBER_FORMAT_FIXED_POINT)
	#include "fix16.hpp"
	#include "dsp_management_api.h"
	#include "dsp_management_internal_api.h"
	real_t pow_fix_point(real_t a, real_t b);
#endif

#endif
