/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "clock_control_i96xxx_hifi3_api.h"
#include 	"src/clock_control_i96xxx_hifi3.h"

#define MODULE_CONFIG_DATA_STRUCT_TYPE	struct clk_cntl_i96xxx_hifi3_cfg_t


#ifndef CLOCK_CONTROL_I96XXX_HIFI3_DSP_RATE
	#error "CLOCK_CONTROL_I96XXX_HIFI3X_DSP_RATE should be defined"
#endif



#define STATIC_DEV_DATA_STRUCT {        \
	CLOCK_CONTROL_I96XXX_HIFI3_DSP_RATE,     \
	}

#include "add_static_dev.h"

/*
 *  device specific defines should be undefined
 *  after calling #include "add_static_dev.h"
 */
#undef CLOCK_CONTROL_I96XXX_HIFI3_DSP_RATE
