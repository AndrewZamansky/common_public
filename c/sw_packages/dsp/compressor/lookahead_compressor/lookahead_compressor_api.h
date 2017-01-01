
#ifndef _lookahead_compressor_API_H_
#define _lookahead_compressor_API_H_

#include "src/_lookahead_compressor_prerequirements_check.h"


/*****************  defines  **************/


/**********  define API  types ************/

typedef enum
{
	LOOKAHEAD_COMPRESSOR_ERR_DIVISIBILITY = 0x0001
}LOOKAHEAD_COMPRESSOR_API_errors_t;


typedef enum
{
	IOCTL_LOOKAHEAD_COMPRESSOR_SET_TYPE = IOCTL_DSP_LAST_COMMON_IOCTL+1,
	IOCTL_LOOKAHEAD_COMPRESSOR_SET_HIGH_THRESHOLD,
	IOCTL_LOOKAHEAD_COMPRESSOR_SET_ATTACK,
	IOCTL_LOOKAHEAD_COMPRESSOR_SET_RELEASE,
	IOCTL_LOOKAHEAD_COMPRESSOR_SET_LOOK_AHEAD_SIZE,
	IOCTL_LOOKAHEAD_COMPRESSOR_SET_RATIO,
	IOCTL_LOOKAHEAD_COMPRESSOR_GET_ERRORS,
	IOCTL_LOOKAHEAD_COMPRESSOR_GET_HIT_COUNTER
}LOOKAHEAD_COMPRESSOR_API_ioctl_t;


typedef enum
{
	LOOKAHEAD_COMPRESSOR_API_TYPE_LIMITER ,
	LOOKAHEAD_COMPRESSOR_API_TYPE_COMPRESSOR
}LOOKAHEAD_COMPRESSOR_API_type_t;



/**********  define API  functions  ************/

extern char lookahead_compressor_module_name[] ;
#define LOOKAHEAD_COMPRESSOR_API_MODULE_NAME	lookahead_compressor_module_name

#endif
