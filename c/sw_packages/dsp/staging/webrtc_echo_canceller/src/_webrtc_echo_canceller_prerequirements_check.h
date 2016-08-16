/*
 * file : webrtc_echo_canceller_prerequirements_check.h
 *
 *
 *
 *
 *
 *
 */

#ifndef _webrtc_echo_canceller_prerequirements_check_H
#define _webrtc_echo_canceller_prerequirements_check_H

#include "_project.h"
#include "dev_management_api.h"
#include "dsp_management_api.h"


/***************************/
/******** test defines *****/



#ifndef WEBRTC_ECHO_CANCELLER_CONFIG_NUM_OF_DYNAMIC_INSTANCES
	#define WEBRTC_ECHO_CANCELLER_CONFIG_NUM_OF_DYNAMIC_INSTANCES 	1
#endif


/***************************/




/***************************/
/*** test type existence (suffix should be unique)***/

typedef uint8_t 					_if_fail_here_then___uint8_t___is_not_defined_in_webrtc_echo_canceller_config_h;
typedef uint16_t		 			_if_fail_here_then___uint16_t___is_not_defined_in_webrtc_echo_canceller_config_h;
typedef uint32_t   					_if_fail_here_then___uint32_t___is_not_defined_in_webrtc_echo_canceller_config_h;
typedef pdsp_descriptor   			_if_fail_here_then___pdsp_descriptor___is_not_defined_in_webrtc_echo_canceller_config_h;
typedef dsp_param_t   				_if_fail_here_then___dsp_param_t___is_not_defined_in_webrtc_echo_canceller_config_h;


typedef ioctl_get_data_buffer_t    	_if_fail_here_then___ioctl_get_data_buffer_t___is_not_defined_in_webrtc_echo_canceller_config_h;

/***************************/


/**********************************/
/*** test variable/function declaration existence (suffix should be unique)***/

enum
{
#ifndef IOCTL_DSP_LAST_COMMON_IOCTL
	IOCTL_LAST_COMMON_IOCTL_not_declared_in_webrtc_echo_canceller_config_h=sizeof((int)IOCTL_DSP_LAST_COMMON_IOCTL),
#endif
#ifndef DEV_CALLBACK_1_PARAMS
	DEV_CALLBACK_1_PARAMS_not_declared_in_webrtc_echo_canceller_config_h=sizeof((int)DEV_CALLBACK_1_PARAMS),
#endif




#ifdef WEBRTC_ECHO_CANCELLER_CONFIG_USE_MALLOC
	memcpy___not_declared_in_webrtc_echo_canceller_config_h=sizeof((int)memcpy),
	malloc___not_declared_in_webrtc_echo_canceller_config_h=sizeof((int)malloc),
	free___not_declared_in_webrtc_echo_canceller_config_h=sizeof((int)free),
#endif

#ifndef busy_delay
	busy_delay___declared_in_webrtc_echo_canceller_config_h=sizeof((int)busy_delay)
#endif


};

/***************************/



#endif /* */
