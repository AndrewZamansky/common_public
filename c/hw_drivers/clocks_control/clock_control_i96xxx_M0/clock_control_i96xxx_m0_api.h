
#ifndef _CLOCK_CONTROL_I96XXX_M0_API_H_
#define _CLOCK_CONTROL_I96XXX_M0_API_H_

#include "dev_management_api.h"
#include "clock_control_api.h"

extern struct dev_desc_t  *i96xxx_xtal_clk_dev;
extern struct dev_desc_t  *i96xxx_lxtal_clk_dev;
extern struct dev_desc_t  *i96xxx_hirc_clk_dev;
extern struct dev_desc_t  *i96xxx_pll_clk_dev;
extern struct dev_desc_t  *i96xxx_hclk_clk_dev;
extern struct dev_desc_t  *i96xxx_systick_clk_dev;
extern struct dev_desc_t  *i9xxxx_uart0_clk_dev;
extern struct dev_desc_t  *i96xxx_i2s0_clk_dev;
extern struct dev_desc_t  *i96xxx_I2S0_MCLK_clk_dev;
extern struct dev_desc_t  *i96xxx_I2S0_BCLK_clk_dev;
extern struct dev_desc_t  *i96xxx_I2S0_FSCLK_clk_dev;
extern struct dev_desc_t  *i96xxx_i2s1_clk_dev;
extern struct dev_desc_t  *i96xxx_I2S1_MCLK_clk_dev;
extern struct dev_desc_t  *i96xxx_I2S1_BCLK_clk_dev;
extern struct dev_desc_t  *i96xxx_I2S1_FSCLK_clk_dev;
extern struct dev_desc_t  *i96xxx_pclk0_clk_dev;
extern struct dev_desc_t  *i96xxx_pclk1_clk_dev;
extern struct dev_desc_t  *i96xxx_dsp_clk_dev;
extern struct dev_desc_t  *i96xxx_usb_clk_dev;
extern struct dev_desc_t  *i96xxx_i2c0_clk_dev;

struct clk_cntl_i96xxx_m0_cfg_t {
	uint32_t           xtal_rate;
	uint32_t           hirc_rate;
	uint32_t           pll_rate;
	uint32_t           hclk_rate;
	uint32_t           pclk0_rate;
	uint32_t           pclk1_rate;
	uint32_t           dsp_rate;
	struct dev_desc_t  *pll_src_clk_dev;
	struct dev_desc_t  *hclk_src_clk_dev;
};

SET_CONFIG_TYPE(clock_control_i96xxx_m0, struct clk_cntl_i96xxx_m0_cfg_t);

#endif
