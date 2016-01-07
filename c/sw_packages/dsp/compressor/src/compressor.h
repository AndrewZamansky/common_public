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

#include "compressor_config.h"
#include "dev_managment_api.h" // for device manager defines and typedefs
#include "src/_compressor_prerequirements_check.h" // should be after {compressor_config.h,dev_managment_api.h}


/***************   typedefs    *******************/



typedef struct {

	uint8_t type;
	uint32_t latency;
	float *latency_buffer_Ch1;
	float *latency_buffer_Ch2;
	float threshold;
	float reverse_ratio;
	float prev_ratio;
	float attack;
	float release;
} COMPRESSOR_Instance_t;




#endif /* */
