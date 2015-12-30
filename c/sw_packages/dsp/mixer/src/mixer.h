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

#include "mixer_config.h"
#include "dev_managment_api.h" // for device manager defines and typedefs
#include "src/_mixer_prerequirements_check.h" // should be after {mixer_config.h,dev_managment_api.h}


/***************   typedefs    *******************/


typedef struct {

	uint8_t num_of_channels ;
	float *channels_weights;

} MIXER_Instance_t;




#endif /* */
