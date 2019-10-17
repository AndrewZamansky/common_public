/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "clock_control_i96xxx_hifi3_api.h"
#include 	"src/clock_control_i96xxx_hifi3.h"


#ifndef CLOCK_CONTROL_I96XXX_HIFI3_DSP_RATE
	#error "CLOCK_CONTROL_I96XXX_HIFI3X_DSP_RATE should be defined"
#endif


SET_STATIC_DEV_CONFIG(clock_control_i96xxx_hifi3) =
{
	CLOCK_CONTROL_I96XXX_HIFI3_DSP_RATE,
};

#define MODULE_HAS_NO_RUNTIME_DATA

#include "add_static_dev.h"

/*
 *  device specific defines should be undefined
 *  after calling #include "add_static_dev.h"
 */
#undef CLOCK_CONTROL_I96XXX_HIFI3_DSP_RATE
