/*
 * file : dev_management_prerequirements_check.h
 *
 *
 *
 *
 *
 *
 */

#ifndef _dev_management_prerequirements_check_H
#define _dev_management_prerequirements_check_H


#include "_project.h"
#include "arch.h"

/***************************/
/******** test defines *****/



#ifndef CONFIG_MAX_NUM_OF_DYNAMIC_DEVICES
	#define CONFIG_MAX_NUM_OF_DYNAMIC_DEVICES 	16
#endif



#ifndef MAX_DEV_NAME_LEN
	#define MAX_DEV_NAME_LEN			8
#endif

#ifndef NULL
	#error  "NULL is not defined in project header files "
#endif

/***************************/




/***************************/
/*** test type existence (suffix should be unique)***/

typedef uint8_t _if_fail_here_then___uint8_t___is_not_defined_in_project_header_files_1DEVM1;
typedef uint16_t _if_fail_here_then___uint16_t___is_not_defined_in_project_header_files_1DEVM1;
typedef uint32_t _if_fail_here_then___uint32_t___is_not_defined_in_project_header_files_1DEVM1;

/***************************/


/**********************************/
/*** test variable/function declaration existence (suffix should be unique)***/


enum
{
	strcmp_not_declared_in_project_header_files_1DEVM1=sizeof((int)strcmp)
};

#if DEV_CONFIG_MAX_NUM_OF_DYNAMIC_DEVICES > 0
	enum
	{
		snprintf___not_declared_in_project_header_files_1DEVM1=sizeof((int)snprintf),
		strnlen___not_declared_in_project_header_files_1DEVM1=sizeof((int)strnlen),
		strcpy___not_declared_in_project_header_files_1DEVM1=sizeof((int)strcpy)
	};
#endif


/***************************/


#endif /* */
