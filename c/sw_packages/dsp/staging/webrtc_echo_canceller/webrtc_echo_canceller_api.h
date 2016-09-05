
#ifndef _webrtc_echo_canceller_API_H_
#define _webrtc_echo_canceller_API_H_


#include "src/_webrtc_echo_canceller_prerequirements_check.h" // should be after {webrtc_echo_canceller_config.h,dev_management_api.h}

#include "common_dsp_api.h"

/*****************  defines  **************/


/**********  define API  types ************/




/**********  define API  functions  ************/


extern "C" uint8_t  webrtc_echo_canceller_api_init_dsp_descriptor(pdsp_descriptor aDspDescriptor);

#endif
