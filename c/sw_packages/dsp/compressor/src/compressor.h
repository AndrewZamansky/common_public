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

	uint8_t dummy;
	float threshold;
	float reverse_ratio;

} COMPRESSOR_Instance_t;




#endif /* */
