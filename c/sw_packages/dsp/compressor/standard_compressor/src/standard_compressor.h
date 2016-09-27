/*
 * file : STANDARD_COMPRESSOR.h
 *
 *
 *
 *
 *
 *
 */

#ifndef _STANDARD_COMPRESSOR_H
#define _STANDARD_COMPRESSOR_H

#include "src/_standard_compressor_prerequirements_check.h"


/***************   typedefs    *******************/



typedef struct {

	float env_follower;
	float rms;
	float threshold;
	float reverse_ratio;
	float attack;
	float release;
	float gain;
	float alpha ;
	float one_minus_alpha;

} STANDARD_COMPRESSOR_Instance_t;




#endif /* */
