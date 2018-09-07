/*
 *
 * file :   global_prerequirements_check.c
 *
 *
 *
 */



/********  includes *********************/
#include "_project.h"
//#include "dev_management_api.h"
#if !defined(CONFIG_HOST) && !defined(CONFIG_ANDROID_NDK)
    #include "os_wrapper.h"
#endif
#include "global_prerequirements_check.h"


/********  defines *********************/


/********  types  *********************/

/**********************************/
/*** test variable/function declaration existence (suffix should be unique)***/

enum
{
//#ifndef IOCTL_LAST_COMMON_IOCTL
//	IOCTL_LAST_COMMON_IOCTL_not_declared_in_project_header_files =sizeof((int)IOCTL_LAST_COMMON_IOCTL),
//#endif
//#ifndef DEV_CALLBACK_1_PARAMS
//	DEV_CALLBACK_1_PARAMS_not_declared_in_project_header_files =sizeof((int)DEV_CALLBACK_1_PARAMS),
//#endif


#if !defined(CONFIG_HOST) && !defined(CONFIG_ANDROID_NDK)
	#ifndef os_stack_test
		os_stack_test___not_declared_in_shell_config_h=sizeof((size_t)os_stack_test),
	#endif
#endif

#ifdef GLOBAL_CONFIG_USE_MALLOC
	memcpy___not_declared_in_project_header_files =sizeof((size_t)memcpy),
	malloc___not_declared_in_project_header_files =sizeof((size_t)malloc),
	free___not_declared_in_project_header_files =sizeof((size_t)free),
#endif

	strcmp_not_declared_in_project_header_files =sizeof((size_t)strcmp)



};


/********  externals *********************/


/********  local defs *********************/




/**********   external variables    **************/



/***********   local variables    **************/

