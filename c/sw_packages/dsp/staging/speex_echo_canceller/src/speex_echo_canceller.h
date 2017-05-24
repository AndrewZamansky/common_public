/*
 * file : SPEEX_ECHO_CANCELLER.h
 *
 *
 */

#ifndef _SPEEX_ECHO_CANCELLER_H
#define _SPEEX_ECHO_CANCELLER_H


/***************   typedefs    *******************/


#include "speex/speex_echo.h"
#include "speex/speex_preprocess.h"

struct SPEEX_ECHO_CANCELLER_Instance_t {
   SpeexEchoState *echo_state;
   SpeexPreprocessState *preprocess_state;
};




#endif /* */
