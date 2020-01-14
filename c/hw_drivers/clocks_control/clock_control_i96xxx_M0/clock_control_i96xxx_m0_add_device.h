/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include "clock_control_i96xxx_m0_api.h"
#include "src/clock_control_i96xxx_m0.h"

#ifndef CLOCK_CONTROL_I96XXX_XTAL_RATE
	#error "CLOCK_CONTROL_I96XXX_XTAL_RATE should be defined"
#endif

#ifndef CLOCK_CONTROL_I96XXX_HIRC_RATE
	#error "CLOCK_CONTROL_I96XXX_HIRC_RATE should be defined"
#endif

#ifndef CLOCK_CONTROL_I96XXX_DT_PLL_RATE
	#error "CLOCK_CONTROL_I96XXX_DT_PLL_RATE should be defined"
#endif

#ifndef CLOCK_CONTROL_I96XXX_DT_HCLK_RATE
	#error "CLOCK_CONTROL_I96XXX_DT_HCLK_RATE should be defined"
#endif

#ifndef CLOCK_CONTROL_I96XXX_DT_PCLK0_RATE
	#error "CLOCK_CONTROL_I96XXX_DT_PCLK0_RATE should be defined"
#endif

#ifndef CLOCK_CONTROL_I96XXX_DT_PCLK1_RATE
	#error "CLOCK_CONTROL_I96XXX_DT_PCLK1_RATE should be defined"
#endif

#ifndef CLOCK_CONTROL_I96XXX_DT_DSP_RATE
	#error "CLOCK_CONTROL_I96XXX_DT_DSP_RATE should be defined"
#endif

#ifndef CLOCK_CONTROL_PLL_SRC_CLK_DEV
	#error "CLOCK_CONTROL_PLL_SRC_CLK_DEV should be defined"
#else
	EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(CLOCK_CONTROL_PLL_SRC_CLK_DEV);
	#define _CLOCK_CONTROL_PLL_SRC_CLK_PDEV  \
		P_TO_STATIC_DEVICE_INST(CLOCK_CONTROL_PLL_SRC_CLK_DEV)

#endif

#ifndef CLOCK_CONTROL_HCLK_SRC_CLK_DEV
	#error "CLOCK_CONTROL_HCLK_SRC_CLK_DEV should be defined"
#else
	EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(CLOCK_CONTROL_HCLK_SRC_CLK_DEV);
	#define _CLOCK_CONTROL_HCLK_SRC_CLK_PDEV  \
		P_TO_STATIC_DEVICE_INST(CLOCK_CONTROL_HCLK_SRC_CLK_DEV)

#endif


SET_STATIC_DEV_CONFIG(clock_control_i96xxx_m0) =
{
	CLOCK_CONTROL_I96XXX_XTAL_RATE,
	CLOCK_CONTROL_I96XXX_HIRC_RATE,
	CLOCK_CONTROL_I96XXX_DT_PLL_RATE,
	CLOCK_CONTROL_I96XXX_DT_HCLK_RATE,
	CLOCK_CONTROL_I96XXX_DT_PCLK0_RATE,
	CLOCK_CONTROL_I96XXX_DT_PCLK1_RATE,
	CLOCK_CONTROL_I96XXX_DT_DSP_RATE,
	_CLOCK_CONTROL_PLL_SRC_CLK_PDEV,
	_CLOCK_CONTROL_HCLK_SRC_CLK_PDEV
};

#define MODULE_HAS_NO_RUNTIME_DATA

#include "add_static_dev.h"

/*
 *  device specific defines should be undefined
 *  after calling #include "add_static_dev.h"
 */
#undef CLOCK_CONTROL_I96XXX_XTAL_RATE
#undef CLOCK_CONTROL_I96XXX_HIRC_RATE
#undef CLOCK_CONTROL_I96XXX_DT_PLL_RATE
#undef CLOCK_CONTROL_I96XXX_DT_HCLK_RATE
#undef CLOCK_CONTROL_PLL_SRC_CLK_DEV
#undef CLOCK_CONTROL_HCLK_SRC_CLK_DEV
#undef _CLOCK_CONTROL_PLL_SRC_CLK_PDEV
#undef _CLOCK_CONTROL_HCLK_SRC_CLK_PDEV
#undef CLOCK_CONTROL_I96XXX_DT_PCLK0_RATE
#undef CLOCK_CONTROL_I96XXX_DT_PCLK1_RATE
#undef CLOCK_CONTROL_I96XXX_DT_DSP_RATE
