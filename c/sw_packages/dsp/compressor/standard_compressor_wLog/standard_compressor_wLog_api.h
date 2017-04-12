
#ifndef _standard_compressor_wLog_API_H_
#define _standard_compressor_wLog_API_H_

#include "src/_standard_compressor_wLog_prerequirements_check.h"


/*****************  defines  **************/


/**********  define API  types ************/



typedef enum
{
	IOCTL_STANDARD_COMPRESSOR_wLog_SET_HIGH_THRESHOLD =
										IOCTL_DSP_LAST_COMMON_IOCTL+1,
	IOCTL_STANDARD_COMPRESSOR_wLog_SET_ATTACK,
	IOCTL_STANDARD_COMPRESSOR_wLog_SET_RELEASE,
	IOCTL_STANDARD_COMPRESSOR_wLog_SET_RATIO,
}STANDARD_COMPRESSOR_wLog_API_ioctl_t;



/**********  define API  functions  ************/

extern char standard_compressor_wLog_module_name[] ;
#define STANDARD_COMPRESSOR_wLog_API_MODULE_NAME	\
							standard_compressor_wLog_module_name

#endif
