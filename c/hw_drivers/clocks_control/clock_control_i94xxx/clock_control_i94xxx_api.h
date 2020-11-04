
#ifndef _CLOCK_CONTROL_I94XXX_API_H_
#define _CLOCK_CONTROL_I94XXX_API_H_

#include "dev_management_api.h"
#include "clock_control_api.h"


#define CLOCK_I9XXXX_API_EXT_TYPE_DUMMY    0
#define CLOCK_I9XXXX_API_EXT_TYPE_XTAL     1
#define CLOCK_I9XXXX_API_EXT_TYPE_GPIO     2

extern struct dev_desc_t  *i94xxx_ext_clk_dev;
extern struct dev_desc_t  *i94xxx_lext_clk_dev;
extern struct dev_desc_t  *i94xxx_hirc_clk_dev;
extern struct dev_desc_t  *i94xxx_pll_clk_dev;
extern struct dev_desc_t  *i94xxx_hclk_clk_dev;
extern struct dev_desc_t  *i94xxx_pclk0_clk_dev;
extern struct dev_desc_t  *i94xxx_pclk1_clk_dev;
extern struct dev_desc_t  *i94xxx_spi2_clk_dev;
extern struct dev_desc_t  *i94xxx_spi1_clk_dev;
extern struct dev_desc_t  *i9xxxx_uart0_clk_dev;
extern struct dev_desc_t  *i94xxx_systick_clk_dev;
extern struct dev_desc_t  *i94xxx_i2s_clk_dev;
extern struct dev_desc_t  *i94xxx_dpwm_clk_dev;
extern struct dev_desc_t  *i94xxx_i2c0_clk_dev;
extern struct dev_desc_t  *i94xxx_i2c1_clk_dev;
extern struct dev_desc_t  *i94xxx_spi0_clk_dev;
extern struct dev_desc_t  *i94xxx_usb_clk_dev;
extern struct dev_desc_t  *i94xxx_I2S_MCLK_clk_dev;
extern struct dev_desc_t  *i94xxx_I2S_BCLK_clk_dev;
extern struct dev_desc_t  *i94xxx_I2S_FSCLK_clk_dev;
extern struct dev_desc_t  *i94xxx_I2S_onSPI1_MCLK_clk_dev;
extern struct dev_desc_t  *i94xxx_I2S_onSPI1_BCLK_clk_dev;
extern struct dev_desc_t  *i94xxx_I2S_onSPI1_FSCLK_clk_dev;
extern struct dev_desc_t  *i94xxx_I2S_onSPI2_MCLK_clk_dev;
extern struct dev_desc_t  *i94xxx_I2S_onSPI2_BCLK_clk_dev;
extern struct dev_desc_t  *i94xxx_I2S_onSPI2_FSCLK_clk_dev;
extern struct dev_desc_t  *i94xxx_dmic_clk_dev;

struct clk_cntl_i94xxx_cfg_t {
	uint32_t           ext_rate;
	uint32_t           hirc_rate;
	uint32_t           pll_rate;
	uint32_t           hclk_rate;
	struct dev_desc_t  *pll_src_clk_dev;
	uint8_t            ext_clock_type;
};
SET_CONFIG_TYPE(clock_control_i94xxx, struct clk_cntl_i94xxx_cfg_t);

#endif
