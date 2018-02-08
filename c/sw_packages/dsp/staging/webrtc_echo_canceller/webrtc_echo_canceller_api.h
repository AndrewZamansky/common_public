
#ifndef _webrtc_echo_canceller_API_H_
#define _webrtc_echo_canceller_API_H_


#include "dsp_management_api.h"

/*****************  defines  **************/


/**********  define API  types ************/




/**********  define API  functions  ************/


extern "C" uint8_t  webrtc_echo_canceller_api_init_dsp_descriptor(struct dsp_module_inst_t aDspDescriptor);

#endif
