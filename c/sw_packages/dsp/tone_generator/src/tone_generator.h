/*
 * file : TONE_GENERATOR.h
 *
 */

#ifndef _TONE_GENERATOR_H
#define _TONE_GENERATOR_H



/***************   typedefs    *******************/



struct TONE_GENERATOR_Instance_t {
	uint32_t freq_hz;
	uint32_t sample_rate_hz;
	real_t volume_lin;
	real_t curr_time;
	real_t curr_time_delta;
	real_t prev_val;
};




#endif
