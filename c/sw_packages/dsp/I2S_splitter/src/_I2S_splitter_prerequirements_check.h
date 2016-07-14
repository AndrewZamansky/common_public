/*
 * file : I2S_splitter_prerequirements_check.h
 *
 *
 *
 *
 *
 *
 */

#ifndef _I2S_splitter_prerequirements_check_H
#define _I2S_splitter_prerequirements_check_H

#include "_project.h"
#include "dev_management_api.h"
#include "dsp_management_api.h"

/***************************/
/******** test defines *****/



#ifndef NUM_OF_BYTES_PER_AUDIO_WORD
	#error "NUM_OF_BYTES_PER_AUDIO_WORD should be defined in project header files"
#endif

/***************************/



/***************************/
/*** test type existence (suffix should be unique)***/

typedef uint8_t 					_if_fail_here_then___uint8_t___is_not_defined_in_project_header_files_1I2SSPLITTER1;
typedef uint16_t		 			_if_fail_here_then___uint16_t___is_not_defined_in_project_header_files_1I2SSPLITTER1;
typedef uint32_t   					_if_fail_here_then___uint32_t___is_not_defined_in_project_header_files_1I2SSPLITTER1;
typedef pdsp_descriptor   			_if_fail_here_then___pdsp_descriptor___is_not_defined_in_project_header_files_1I2SSPLITTER1;
typedef dsp_param_t   				_if_fail_here_then___dsp_param_t___is_not_defined_in_project_header_files_1I2SSPLITTER1;


typedef ioctl_get_data_buffer_t    	_if_fail_here_then___ioctl_get_data_buffer_t___is_not_defined_in_project_header_files_1I2SSPLITTER1;

/***************************/


/**********************************/
/*** test variable/function declaration existence (suffix should be unique)***/

enum
{
#ifndef IOCTL_LAST_COMMON_IOCTL
	IOCTL_LAST_COMMON_IOCTL_not_declared_in_project_header_files_1I2SSPLITTER1=sizeof((int)IOCTL_LAST_COMMON_IOCTL),
#endif
#ifndef DEV_CALLBACK_1_PARAMS
	DEV_CALLBACK_1_PARAMS_not_declared_in_project_header_files_1I2SSPLITTER1=sizeof((int)DEV_CALLBACK_1_PARAMS),
#endif



	memcpy___not_declared_in_project_header_files_1I2SSPLITTER1=sizeof((int)memcpy),
	malloc___not_declared_in_project_header_files_1I2SSPLITTER1=sizeof((int)malloc),
	free___not_declared_in_project_header_files_1I2SSPLITTER1=sizeof((int)free)


};

/***************************/



#endif /* */
