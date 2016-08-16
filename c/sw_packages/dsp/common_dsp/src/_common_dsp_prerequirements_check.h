/*
 * file : common_dsp_prerequirements_check.h
 *
 *
 *
 *
 *
 *
 */

#ifndef _common_dsp_prerequirements_check_H
#define _common_dsp_prerequirements_check_H



/***************************/
/******** test defines *****/



#ifndef COMMON_DSP_CONFIG_NUM_OF_DYNAMIC_INSTANCES
	#define COMMON_DSP_CONFIG_NUM_OF_DYNAMIC_INSTANCES 	1
#endif


/***************************/




/***************************/
/*** test type existence (suffix should be unique)***/

typedef uint8_t 					_if_fail_here_then___uint8_t___is_not_defined_in_project_header_files;
typedef uint16_t		 			_if_fail_here_then___uint16_t___is_not_defined_in_project_header_files;
typedef uint32_t   					_if_fail_here_then___uint32_t___is_not_defined_in_project_header_files;



/***************************/


/**********************************/
/*** test variable/function declaration existence (suffix should be unique)***/

enum
{
#ifndef IOCTL_DSP_LAST_COMMON_IOCTL
//	IOCTL_LAST_COMMON_IOCTL_not_declared_in_project_header_files=sizeof((int)IOCTL_DSP_LAST_COMMON_IOCTL),
#endif
#ifndef DEV_CALLBACK_1_PARAMS
//	DEV_CALLBACK_1_PARAMS_not_declared_in_project_header_files=sizeof((int)DEV_CALLBACK_1_PARAMS),
#endif





#ifdef COMMON_DSP_CONFIG_USE_MALLOC
	memcpy___not_declared_in_project_header_files=sizeof((int)memcpy),
	malloc___not_declared_in_project_header_files=sizeof((int)malloc),
	free___not_declared_in_project_header_files=sizeof((int)free),
#endif

#ifndef busy_delay
	busy_delay___declared_in_project_header_files=sizeof((int)busy_delay)
#endif


};

/***************************/



#endif /* */
