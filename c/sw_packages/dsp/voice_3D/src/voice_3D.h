/*
 * file : VOICE_3D.h
 *
 *
 *
 *
 *
 *
 */

#ifndef _VOICE_3D_H
#define _VOICE_3D_H

#include "voice_3D_config.h"
#include "dev_managment_api.h" // for device manager defines and typedefs
#include "src/_voice_3D_prerequirements_check.h" // should be after {voice_3D_config.h,dev_managment_api.h}


/***************   typedefs    *******************/



typedef struct
{
	float medium_gain;
	float side_gain;
	float _3D_gain;
} VOICE_3D_Instance_t;




#endif /* */
