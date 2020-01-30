
#ifndef _CLOCK_CONTROL_STM32f10X_API_H_
#define _CLOCK_CONTROL_STM32f10X_API_H_

#include "dev_management_api.h"
#include "clock_control_api.h"

extern struct dev_desc_t  *stm32f10x_xtal_clk_dev;
extern struct dev_desc_t  *stm32f10x_hsirc_clk_dev;
extern struct dev_desc_t  *stm32f10x_sysclk_dev;
extern struct dev_desc_t  *stm32f10x_hclk_clk_dev;
extern struct dev_desc_t  *stm32f10x_systick_clk_dev;
extern struct dev_desc_t  *stm32f10x_apb2_clk_dev;
extern struct dev_desc_t  *stm32f10x_adc_clk_dev;

struct clk_cntl_stm32f10x_cfg_t {
	uint32_t           hsirc_rate;
	uint32_t           xtal_rate;
	uint32_t           pll_rate;
	uint32_t           hclk_rate;
	uint32_t           apb2_rate;
	struct dev_desc_t  *pll_src_clk_dev;
	struct dev_desc_t  *sysclk_src_clk_dev;
};
SET_CONFIG_TYPE(clock_control_stm32f10x, struct clk_cntl_stm32f10x_cfg_t);

#endif
