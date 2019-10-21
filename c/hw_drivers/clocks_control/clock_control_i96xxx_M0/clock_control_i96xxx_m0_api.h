
#ifndef _CLOCK_CONTROL_I96XXX_M0_API_H_
#define _CLOCK_CONTROL_I96XXX_M0_API_H_

#include "dev_management_api.h"
#include "clock_control_api.h"

/**********  define API  types ************/


/**********  define API  functions  ************/

extern struct dev_desc_t  *i96xxx_xtal_clk_dev;
extern struct dev_desc_t  *i96xxx_lxtal_clk_dev;
extern struct dev_desc_t  *i96xxx_hirc_clk_dev;
extern struct dev_desc_t  *i96xxx_pll_clk_dev;
extern struct dev_desc_t  *i96xxx_hclk_clk_dev;
extern struct dev_desc_t  *i96xxx_systick_clk_dev;

struct clk_cntl_i96xxx_m0_cfg_t {
	uint32_t           xtal_rate;
	uint32_t           hirc_rate;
	uint32_t           pll_rate;
	uint32_t           hclk_rate;
	struct dev_desc_t  *pll_src_clk_dev;
	struct dev_desc_t  *hclk_src_clk_dev;
};

SET_CONFIG_TYPE(clock_control_i96xxx_m0, struct clk_cntl_i96xxx_m0_cfg_t);

#endif
