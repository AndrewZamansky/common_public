
#ifndef _CLOCK_CONTROL_API_H_
#define _CLOCK_CONTROL_API_H_

#include "dev_management_api.h" // for device manager defines and typedefs

/*****************  defines  **************/


/**********  define API  types ************/


/**********  define API  functions  ************/
typedef enum
{
	IOCTL_CLOCK_CONTROL_GET_RATE = IOCTL_LAST_COMMON_IOCTL+1,
	IOCTL_CLOCK_CONTROL_SET_RATE,
}CLOCK_CONTROL_COMMON_API_ioctl_t;



#endif
