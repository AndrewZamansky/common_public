/*
 * file : COMPRESSOR.h
 *
 *
 *
 *
 *
 *
 */

#ifndef _COMPRESSOR_H
#define _COMPRESSOR_H

#include "src/_compressor_prerequirements_check.h"


/***************   typedefs    *******************/



typedef struct {

	uint8_t type;
	uint8_t usePreviousRatio;
	uint32_t look_ahead_length;
	float *look_ahead_length_buffer_Ch1;
	float *look_ahead_length_buffer_Ch2;
	float threshold;
	float reverse_ratio;
	float prev_ratio;
	float attack;
	float release;
	float prev_calculated_ratio;
	uint32_t hit_counter;
} COMPRESSOR_Instance_t;




#endif /* */
