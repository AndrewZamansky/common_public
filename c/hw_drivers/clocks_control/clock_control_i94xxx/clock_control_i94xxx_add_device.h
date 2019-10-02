/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "clock_control_i94xxx_api.h"
#include 	"src/clock_control_i94xxx.h"

#define MODULE_CONFIG_DATA_STRUCT_TYPE	struct clk_cntl_i94xxx_cfg_t


#ifndef CLOCK_CONTROL_I94XXX_XTAL_RATE
	#error "CLOCK_CONTROL_I94XXX_XTAL_RATE should be defined"
#endif

#ifndef CLOCK_CONTROL_I94XXX_HIRC_RATE
	#error "CLOCK_CONTROL_I94XXX_HIRC_RATE should be defined"
#endif

#ifndef CLOCK_CONTROL_I94XXX_DT_PLL_RATE
	#error "CLOCK_CONTROL_I94XXX_DT_PLL_RATE should be defined"
#endif

#ifndef CLOCK_CONTROL_I94XXX_DT_HCLK_RATE
	#error "CLOCK_CONTROL_I94XXX_DT_HCLK_RATE should be defined"
#endif

#ifndef CLOCK_CONTROL_PLL_SRC_CLK_DEV
	#error "CLOCK_CONTROL_PLL_SRC_CLK_DEV should be defined"
#else
	EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(CLOCK_CONTROL_PLL_SRC_CLK_DEV);
	#define _CLOCK_CONTROL_SRC_CLK_PDEV  \
		P_TO_STATIC_DEVICE_INST(CLOCK_CONTROL_PLL_SRC_CLK_DEV)

#endif


#define STATIC_DEV_DATA_STRUCT {        \
	CLOCK_CONTROL_I94XXX_XTAL_RATE,     \
	CLOCK_CONTROL_I94XXX_HIRC_RATE,     \
	CLOCK_CONTROL_I94XXX_DT_PLL_RATE,   \
	CLOCK_CONTROL_I94XXX_DT_HCLK_RATE,  \
	_CLOCK_CONTROL_SRC_CLK_PDEV         \
	}

#include "add_static_dev.h"

/*
 *  device specific defines should be undefined
 *  after calling #include "add_static_dev.h"
 */
#undef CLOCK_CONTROL_I94XXX_XTAL_RATE
#undef CLOCK_CONTROL_I94XXX_HIRC_RATE
#undef CLOCK_CONTROL_I94XXX_DT_PLL_RATE
#undef CLOCK_CONTROL_I94XXX_DT_HCLK_RATE
#undef _CLOCK_CONTROL_SRC_CLK_PDEV
#undef CLOCK_CONTROL_PLL_SRC_CLK_DEV
