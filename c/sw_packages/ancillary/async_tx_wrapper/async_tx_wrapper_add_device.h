/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "async_tx_wrapper_api.h"
#include "src/async_tx_wrapper.h"

#ifndef ASYNC_TX_WRAPPER_DT_SERVER_PDEV
	#error "ASYNC_TX_WRAPPER_DT_SERVER_PDEV should be defined"
#endif

#ifdef ASYNC_TX_WRAPPER_DT_CHAR_TIMEOUT
	#define  __ASYNC_TX_WRAPPER_DT_CHAR_TIMEOUT ASYNC_TX_WRAPPER_DT_CHAR_TIMEOUT
#else
	//suitable for >1k baud rate with os tick 1ms
	#define  __ASYNC_TX_WRAPPER_DT_CHAR_TIMEOUT   10
#endif

EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(ASYNC_TX_WRAPPER_DT_SERVER_PDEV) ;

SET_STATIC_DEV_CONFIG(async_tx_wrapper) =
{
	P_TO_STATIC_DEVICE_INST(ASYNC_TX_WRAPPER_DT_SERVER_PDEV),
	__ASYNC_TX_WRAPPER_DT_CHAR_TIMEOUT
};



#include "add_static_dev.h"

/**
 *  device specific defines should be undefined
 *  after calling #include "add_static_dev.h"
 */
#undef ASYNC_TX_WRAPPER_DT_SERVER_PDEV
#undef ASYNC_TX_WRAPPER_DT_CHAR_TIMEOUT
#undef __ASYNC_TX_WRAPPER_DT_CHAR_TIMEOUT
