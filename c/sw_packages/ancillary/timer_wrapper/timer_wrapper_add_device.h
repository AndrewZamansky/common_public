/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "timer_wrapper_api.h"
#include  "src/timer_wrapper.h"


#ifndef TIMER_WRAPPER_DT_HW_TIMER_PDEV
	#error "TIMER_WRAPPER_DT_HW_TIMER_PDEV should be defined"
#endif

EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(TIMER_WRAPPER_DT_HW_TIMER_PDEV) ;
SET_STATIC_DEV_CONFIG(timer_wrapper) =
{
	P_TO_STATIC_DEVICE_INST(TIMER_WRAPPER_DT_HW_TIMER_PDEV)
};

#include "add_static_dev.h"

/**
 *  device specific defines should be undefined
 *  after calling #include "add_static_dev.h"
 */
#undef TIMER_WRAPPER_DT_HW_TIMER_PDEV
