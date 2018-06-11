
#ifndef _CLOCK_CONTROL_I94XXX_API_H_
#define _CLOCK_CONTROL_I94XXX_API_H_

#include "dev_management_api.h"
#include "clock_control_api.h"

/**********  define API  types ************/


/**********  define API  functions  ************/
extern struct dev_desc_t  *i94xxx_xtal_clk_dev;
extern struct dev_desc_t  *i94xxx_lxtal_clk_dev;
extern struct dev_desc_t  *i94xxx_hirc_clk_dev;
extern struct dev_desc_t  *i94xxx_pll_clk_dev;
extern struct dev_desc_t  *i94xxx_hclk_clk_dev;
extern struct dev_desc_t  *i94xxx_pclk0_clk_dev;
extern struct dev_desc_t  *i94xxx_pclk1_clk_dev;
extern struct dev_desc_t  *i94xxx_spi2clk_clk_dev;
extern struct dev_desc_t  *i94xxx_uart0clk_clk_dev;
extern struct dev_desc_t  *i94xxx_systick_clk_dev;
extern struct dev_desc_t  *i94xxx_i2s_clk_dev;
extern struct dev_desc_t  *i94xxx_dpwm_clk_dev;
extern struct dev_desc_t  *i94xxx_i2c1_clk_dev;
extern struct dev_desc_t  *i94xxx_spi0_clk_dev;
extern struct dev_desc_t  *i94xxx_usb_clk_dev;

#endif
