
#ifndef _compressor_API_H_
#define _compressor_API_H_

#include "src/_compressor_prerequirements_check.h"


/*****************  defines  **************/


/**********  define API  types ************/



typedef enum
{
	IOCTL_COMPRESSOR_SET_TYPE = IOCTL_DSP_LAST_COMMON_IOCTL+1,
	IOCTL_COMPRESSOR_SET_HIGH_THRESHOLD,
	IOCTL_COMPRESSOR_SET_ATTACK,
	IOCTL_COMPRESSOR_SET_RELEASE,
	IOCTL_COMPRESSOR_SET_LOOK_AHEAD_SIZE,
	IOCTL_COMPRESSOR_SET_RATIO,
	IOCTL_COMPRESSOR_GET_HIT_COUNTER
}COMPRESSOR_API_ioctl_t;


typedef enum
{
	COMPRESSOR_API_TYPE_REGULAR ,
	COMPRESSOR_API_TYPE_LOOKAHEAD
}COMPRESSOR_API_type_t;



/**********  define API  functions  ************/

extern char compressor_module_name[] ;
#define COMPRESSOR_API_MODULE_NAME	compressor_module_name

#endif
