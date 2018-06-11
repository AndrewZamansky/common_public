/*
 *
 *clock_control_i94xxx.h
 *
 *
 */

#ifndef _CLOCK_CONTROL_I94XXX_H_
#define _CLOCK_CONTROL_I94XXX_H_

/********  includes *********************/


/********  types  *********************/

struct clk_cntl_i94xxx_cfg_t {
	uint32_t           xtal_rate;
	uint32_t           hirc_rate;
	uint32_t           pll_rate;
	uint32_t           hclk_rate;
	struct dev_desc_t  *pll_src_clk_dev;
} ;

#endif
