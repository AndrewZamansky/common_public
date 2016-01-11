/*
 * file : SPEEX_ECHO_CANCELLER.h
 *
 *
 *
 *
 *
 *
 */

#ifndef _SPEEX_ECHO_CANCELLER_H
#define _SPEEX_ECHO_CANCELLER_H

#include "speex_echo_canceller_config.h"
#include "dev_managment_api.h" // for device manager defines and typedefs
#include "src/_speex_echo_canceller_prerequirements_check.h" // should be after {speex_echo_canceller_config.h,dev_managment_api.h}


/***************   typedefs    *******************/


#include "speex/speex_echo.h"
#include "speex/speex_preprocess.h"

typedef struct
{
   SpeexEchoState *echo_state;
   SpeexPreprocessState *preprocess_state;
} SPEEX_ECHO_CANCELLER_Instance_t;




#endif /* */
