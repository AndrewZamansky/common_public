/*
 * file : STANDARD_COMPRESSOR_wLog.h
 *
 *
 *
 *
 *
 *
 */

#ifndef _STANDARD_COMPRESSOR_wLog_H
#define _STANDARD_COMPRESSOR_wLog_H

#include "src/_standard_compressor_wLog_prerequirements_check.h"


/***************   typedefs    *******************/



struct standard_compressor_wLog_Instance_t {

	float env_follower;
	float rms;
	float threshold;
	float reverse_ratio;
	float attack;
	float release;
	float gain;
	float alpha ;
	float one_minus_alpha;
};




#endif /* */
