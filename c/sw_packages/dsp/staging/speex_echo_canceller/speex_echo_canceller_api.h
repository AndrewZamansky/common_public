
#ifndef _speex_echo_canceller_API_H_
#define _speex_echo_canceller_API_H_

#include "dsp_management_api.h"

/*****************  defines  **************/


/**********  define API  types ************/



//typedef enum
//{
//	IOCTL_SPEEX_ECHO_CANCELLER_SET_GAIN = IOCTL_DSP_LAST_COMMON_IOCTL+1,
//}SPEEX_ECHO_CANCELLER_API_ioctl_t;



/**********  define API  functions  ************/


uint8_t  speex_echo_canceller_api_init_dsp_descriptor(struct dsp_desc_t aDspDescriptor);

#endif
