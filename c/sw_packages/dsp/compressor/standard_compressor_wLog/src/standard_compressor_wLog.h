/*
 * file : STANDARD_COMPRESSOR_wLog.h
 *
 *
 *
 */

#ifndef _STANDARD_COMPRESSOR_wLog_H
#define _STANDARD_COMPRESSOR_wLog_H



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
