/*
 * file : MIXER.h
 *
 *
 *
 *
 *
 *
 */

#ifndef _MIXER_H
#define _MIXER_H

#include "src/_mixer_prerequirements_check.h"


/***************   typedefs    *******************/


typedef struct {

	uint8_t num_of_input_channels ;
	float *channels_weights;

} MIXER_Instance_t;




#endif /* */
