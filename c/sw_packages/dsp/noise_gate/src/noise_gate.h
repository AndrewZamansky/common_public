/*
 * file : NOISE_GATE.h
 *
 *
 *
 */

#ifndef _NOISE_GATE_H
#define _NOISE_GATE_H



/***************   typedefs    *******************/



struct std_compressor_instance_t {

	float ratio_env_follower;
	float rms;
	float threshold;
	float attack;
	float release;
	float alpha ;
	float one_minus_alpha;

};




#endif /* */
