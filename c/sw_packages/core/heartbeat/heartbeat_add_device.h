/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include "heartbeat_api.h"
#include "src/heartbeat.h"


#ifndef HEARTBEAT_DT_CALLBACK_PDEV
	#error "HEARTBEAT_DT_CALLBACK_PDEV should be defined"
#endif

#ifndef HEARTBEAT_DT_OS_TIMER_PDEV
	#error "HEARTBEAT_DT_OS_TIMER_PDEV should be defined"
#endif

EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(HEARTBEAT_DT_OS_TIMER_PDEV) ;
EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(HEARTBEAT_DT_CALLBACK_PDEV) ;

SET_STATIC_DEV_CONFIG(heartbeat) =
{
	P_TO_STATIC_DEVICE_INST(HEARTBEAT_DT_CALLBACK_PDEV),
	P_TO_STATIC_DEVICE_INST(HEARTBEAT_DT_OS_TIMER_PDEV),
};

#define MODULE_HAS_NO_RUNTIME_DATA

#include "add_static_dev.h"

/**
 *  device specific defines should be undefined
 *  after calling #include "add_static_dev.h"
 */
#undef HEARTBEAT_DT_CALLBACK_FUNC
#undef HEARTBEAT_DT_OS_TIMER_PDEV
