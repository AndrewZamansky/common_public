/*
 * file : EQUALIZER.h
 *
 *
 *
 *
 *
 *
 */

#ifndef _EQUALIZER_H
#define _EQUALIZER_H

#include "equalizer_config.h"
#include "dev_managment_api.h" // for device manager defines and typedefs
#include "src/_equalizer_prerequirements_check.h" // should be after {equalizer_config.h,dev_managment_api.h}


/***************   typedefs    *******************/

typedef struct _BandCoeffs_t
{
	float b0, b1 , b2 , a1 , a2;
} BandCoeffs_t;


typedef struct {

	void *pBiquadFilter   ;
	size_t num_of_bands ;
	BandCoeffs_t *pCoeffs;
	equalizer_api_band_set_params_t band_set_params;
} EQUALIZER_Instance_t;




#endif /* */
