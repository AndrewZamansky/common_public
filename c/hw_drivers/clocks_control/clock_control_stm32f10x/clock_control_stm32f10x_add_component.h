/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "clock_control_stm32f10x_api.h"
#include 	"src/clock_control_stm32f10x.h"

#define	MODULE_NAME						clock_control_stm32f10x
#define	MODULE_IOCTL_FUNCTION			clock_control_stm32f10x_ioctl
#define MODULE_CONFIG_DATA_STRUCT_TYPE	struct clk_cntl_stm32f10x_cfg_t

#ifdef DT_DEV_MODULE

	#ifndef CLOCK_CONTROL_STM32f10X_HIRC_RATE
		#error "CLOCK_CONTROL_STM32f10X_HIRC_RATE should be defined"
	#endif

	#ifndef CLOCK_CONTROL_STM32f10X_XTAL_RATE
		#define CLOCK_CONTROL_STM32f10X_XTAL_RATE  0
		#warning "CLOCK_CONTROL_STM32f10X_XTAL_RATE not defined"
	#endif

	#ifndef CLOCK_CONTROL_STM32f10X_DT_PLL_RATE
		#define CLOCK_CONTROL_STM32f10X_DT_PLL_RATE 0
	#endif
	#if (0 == CLOCK_CONTROL_STM32f10X_DT_PLL_RATE)
		#define _CLOCK_CONTROL_PLL_SRC_CLK_PDEV   NULL
	#else
		#if (!defined(CLOCK_CONTROL_STM32f10X_DT_PLL_SRC_CLK_DEV))
			#error "CLOCK_CONTROL_STM32f10X_DT_PLL_SRC_CLK_DEV should be defined"
		#else
			EXTERN_DECLARATION_TO_STATIC_DEVICE_INST( \
									CLOCK_CONTROL_STM32f10X_DT_PLL_SRC_CLK_DEV);
			#define _CLOCK_CONTROL_PLL_SRC_CLK_PDEV  P_TO_STATIC_DEVICE_INST(\
						CLOCK_CONTROL_STM32f10X_DT_PLL_SRC_CLK_DEV)

		#endif
	#endif

	#ifndef CLOCK_CONTROL_STM32f10X_DT_HCLK_RATE
		#error "CLOCK_CONTROL_STM32f10X_DT_HCLK_RATE should be defined"
	#endif


	#if !defined(CLOCK_CONTROL_STM32f10X_DT_SYSCLK_SRC_CLK_DEV)
		#error "CLOCK_CONTROL_STM32f10X_DT_SYSCLK_SRC_CLK_DEV should be defined"
	#else
		EXTERN_DECLARATION_TO_STATIC_DEVICE_INST( \
							CLOCK_CONTROL_STM32f10X_DT_SYSCLK_SRC_CLK_DEV);
		#define _CLOCK_CONTROL_SYSCLK_SRC_CLK_PDEV  P_TO_STATIC_DEVICE_INST( \
							CLOCK_CONTROL_STM32f10X_DT_SYSCLK_SRC_CLK_DEV)

	#endif

	#define STATIC_DEV_DATA_STRUCT {        \
		CLOCK_CONTROL_STM32f10X_HIRC_RATE,     \
		CLOCK_CONTROL_STM32f10X_XTAL_RATE,     \
		CLOCK_CONTROL_STM32f10X_DT_PLL_RATE,   \
		CLOCK_CONTROL_STM32f10X_DT_HCLK_RATE,  \
		_CLOCK_CONTROL_PLL_SRC_CLK_PDEV  ,     \
		_CLOCK_CONTROL_SYSCLK_SRC_CLK_PDEV     \
		}
#endif

#include "add_component.h"

/*
 *  device specific defines should be undefined
 *  after calling #include "add_component.h"
 */
#undef CLOCK_CONTROL_STM32f10X_XTAL_RATE
#undef CLOCK_CONTROL_STM32f10X_HIRC_RATE
#undef CLOCK_CONTROL_STM32f10X_DT_PLL_RATE
#undef CLOCK_CONTROL_STM32f10X_DT_HCLK_RATE
#undef CLOCK_CONTROL_STM32f10X_DT_PLL_SRC_CLK_DEV
#undef _CLOCK_CONTROL_PLL_SRC_CLK_PDEV
#undef CLOCK_CONTROL_STM32f10X_DT_SYSCLK_SRC_CLK_DEV
#undef _CLOCK_CONTROL_SYSCLK_SRC_CLK_PDEV
