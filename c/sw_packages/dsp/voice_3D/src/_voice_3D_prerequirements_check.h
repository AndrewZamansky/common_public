/*
 * file : voice_3D_prerequirements_check.h
 *
 *
 *
 *
 *
 *
 */

#ifndef _voice_3D_prerequirements_check_H
#define _voice_3D_prerequirements_check_H

#include "_project.h"
#include "dsp_management_api.h"
#include "global_prerequirements_check.h"


/***************************/
/******** test defines *****/



#ifndef VOICE_3D_CONFIG_NUM_OF_DYNAMIC_INSTANCES
	#define VOICE_3D_CONFIG_NUM_OF_DYNAMIC_INSTANCES 	1
#endif


/***************************/




/***************************/
/*** test type existence (suffix should be unique)***/

typedef struct dsp_module_inst_t   			_if_fail_here_then___pdsp_descriptor___is_not_defined_in_voice_3D_config_h;



/***************************/


/**********************************/
/*** test variable/function declaration existence (suffix should be unique)***/

enum
{
#ifndef IOCTL_DSP_LAST_COMMON_IOCTL
	IOCTL_LAST_COMMON_IOCTL_not_declared_in_voice_3D_config_h=sizeof((int)IOCTL_DSP_LAST_COMMON_IOCTL),
#endif

#ifdef VOICE_3D_CONFIG_USE_MALLOC
	memcpy___not_declared_in_voice_3D_config_h=sizeof((int)memcpy),
	malloc___not_declared_in_voice_3D_config_h=sizeof((int)malloc),
	free___not_declared_in_voice_3D_config_h=sizeof((int)free),
#endif


};

/***************************/



#endif /* */
