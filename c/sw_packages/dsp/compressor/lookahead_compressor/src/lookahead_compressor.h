/*
 * file : LOOKAHEAD_COMPRESSOR.h
 *
 *
 *
 *
 *
 *
 */

#ifndef _LOOKAHEAD_COMPRESSOR_H
#define _LOOKAHEAD_COMPRESSOR_H

#include "src/_lookahead_compressor_prerequirements_check.h"


/***************   typedefs    *******************/



typedef struct {

	uint8_t type;
	uint8_t usePreviousRatio;
	uint8_t bChangeChunkSize;
	uint32_t look_ahead_length;
	uint32_t u32Errors;
	float *look_ahead_length_buffer_Ch1;
	float *look_ahead_length_buffer_Ch2;
	float threshold;
	float reverse_ratio;
	float prev_ratio;
	float attack;
	float release;
	float prev_calculated_ratio;
	uint32_t hit_counter;
	float release_ratio_change_per_chunk;
} LOOKAHEAD_COMPRESSOR_Instance_t;




#endif /* */
