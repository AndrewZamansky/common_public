/*
 *
 *clock_control_stm32f10x.h
 *
 *
 */

#ifndef _CLOCK_CONTROL_STM32f10X_H_
#define _CLOCK_CONTROL_STM32f10X_H_

/********  includes *********************/


/********  types  *********************/

struct clk_cntl_stm32f10x_cfg_t {
	uint32_t           hsirc_rate;
	uint32_t           xtal_rate;
	uint32_t           pll_rate;
	uint32_t           hclk_rate;
	uint32_t           apb2_rate;
	struct dev_desc_t  *pll_src_clk_dev;
	struct dev_desc_t  *sysclk_src_clk_dev;
};

#endif
