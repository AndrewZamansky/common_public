/*
 * file : I2S_MIXER.h
 *
 *
 *
 *
 *
 *
 */

#ifndef _I2S_MIXER_H
#define _I2S_MIXER_H

#include "src/_I2S_mixer_prerequirements_check.h"


/***************   typedefs    *******************/


typedef struct
{
	float max_out_val ;
	float output_level ;
	uint8_t enable_test_clipping;

} I2S_MIXER_Instance_t;




#endif /* */
