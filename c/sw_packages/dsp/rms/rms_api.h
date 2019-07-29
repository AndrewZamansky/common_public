
#ifndef _rms_API_H_
#define _rms_API_H_

#include "dsp_management_api.h"

/*****************  defines  **************/


/**********  define API  types ************/



//typedef enum
//{
//	IOCTL_RMS_DUMMY = IOCTL_DSP_LAST_COMMON_IOCTL + 1,
//}RMS_API_ioctl_t;



/**********  define API  functions  ************/

#ifdef __cplusplus
extern "C" {
#endif

extern char rms_module_name[] ;
#define RMS_DSPT	rms_module_name

#ifdef __cplusplus
}
#endif

#endif
