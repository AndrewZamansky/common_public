/*
 * file : standard_compressor_wLog_prerequirements_check.h
 *
 *
 *
 *
 *
 *
 */

#ifndef _standard_compressor_wLog_prerequirements_check_H
#define _standard_compressor_wLog_prerequirements_check_H


#include "_project.h"
#include "dsp_management_api.h"
#include "common_dsp_api.h"

/***************************/
/******** test defines *****/




/***************************/




/***************************/
/*** test type existence (suffix should be unique)***/


/***************************/


/**********************************/
/*** test variable/function declaration existence (suffix should be unique)***/

enum
{
#ifndef IOCTL_DSP_LAST_COMMON_IOCTL
	IOCTL_LAST_COMMON_IOCTL_not_declared_in_standard_compressor_wLog_config_h=sizeof((int)IOCTL_DSP_LAST_COMMON_IOCTL),
#endif



	memcpy___not_declared_in_standard_compressor_wLog_config_h=sizeof((int)memcpy),
	malloc___not_declared_in_standard_compressor_wLog_config_h=sizeof((int)malloc),
	free___not_declared_in_standard_compressor_wLog_config_h=sizeof((int)free)



};

/***************************/



#endif /* */
