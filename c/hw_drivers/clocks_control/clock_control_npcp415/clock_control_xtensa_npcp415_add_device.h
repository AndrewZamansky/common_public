/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "clock_control_xtensa_npcp415_api.h"
#include  "src/clock_control_xtensa_npcp415.h"


#ifndef CLOCK_CONTROL_XTENSA_NPCP415_DT_CORE_RATE
	#error "CLOCK_CONTROL_XTENSA_NPCP415_DT_CORE_RATE should be defined"
#endif


SET_STATIC_DEV_CONFIG(clock_control_xtensa_npcp415) =
{
	CLOCK_CONTROL_XTENSA_NPCP415_DT_CORE_RATE,
};

#define MODULE_HAS_NO_RUNTIME_DATA

#include "add_static_dev.h"

/*
 *  device specific defines should be undefined
 *  after calling #include "add_static_dev.h"
 */

#undef CLOCK_CONTROL_XTENSA_NPCP415_DT_CORE_RATE
