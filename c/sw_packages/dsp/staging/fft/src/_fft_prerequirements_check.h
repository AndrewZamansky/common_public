/*
 * file : fft_prerequirements_check.h
 *
 *
 *
 *
 *
 *
 */

#ifndef _fft_prerequirements_check_H
#define _fft_prerequirements_check_H

#include "_project.h"
#include "dev_management_api.h"
#include "dsp_management_api.h"


/***************************/
/******** test defines *****/



#ifndef FFT_CONFIG_NUM_OF_DYNAMIC_INSTANCES
	#define FFT_CONFIG_NUM_OF_DYNAMIC_INSTANCES 	1
#endif


/***************************/




/***************************/
/*** test type existence (suffix should be unique)***/

typedef uint8_t 					_if_fail_here_then___uint8_t___is_not_defined_in_project_header_files_1FFT1;
typedef uint16_t		 			_if_fail_here_then___uint16_t___is_not_defined_in_project_header_files_1FFT1;
typedef uint32_t   					_if_fail_here_then___uint32_t___is_not_defined_in_project_header_files_1FFT1;
typedef struct dsp_desc_t   			_if_fail_here_then___pdsp_descriptor___is_not_defined_in_project_header_files_1FFT1;



/***************************/


/**********************************/
/*** test variable/function declaration existence (suffix should be unique)***/

enum
{
#ifndef IOCTL_DSP_LAST_COMMON_IOCTL
	IOCTL_LAST_COMMON_IOCTL_not_declared_in_project_header_files_1FFT1=sizeof((int)IOCTL_DSP_LAST_COMMON_IOCTL),
#endif
#ifndef DEV_CALLBACK_1_PARAMS
	DEV_CALLBACK_1_PARAMS_not_declared_in_project_header_files_1FFT1=sizeof((int)DEV_CALLBACK_1_PARAMS),
#endif




#ifdef FFT_CONFIG_USE_MALLOC
	memcpy___not_declared_in_project_header_files_1FFT1=sizeof((int)memcpy),
	malloc___not_declared_in_project_header_files_1FFT1=sizeof((int)malloc),
	free___not_declared_in_project_header_files_1FFT1=sizeof((int)free),
#endif

#ifndef busy_delay
	busy_delay___declared_in_project_header_files_1FFT1=sizeof((int)busy_delay)
#endif


};

/***************************/



#endif /* */
