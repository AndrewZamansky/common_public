#include "_project.h"

#include "dev_management_api.h"


/***********************************/
/********** i94xxx_xtal_clk_dev ********/
#define DT_DEV_NAME				i94xxx_xtal_clk_dev
#define DT_DEV_MODULE			clk_cntl

#define CLK_DT_IOCTL			clock_i94xxx_xtal_ioctl
#define CLK_DT_DEFAULT_RATE		CONFIG_CRYSTAL_CLOCK

#include ADD_CURRENT_DEV


/***********************************/
/********** i94xxx_hirc_clk_dev ********/
#define DT_DEV_NAME				i94xxx_hirc_clk_dev
#define DT_DEV_MODULE			clk_cntl

#define CLK_DT_IOCTL			clock_i94xxx_hirc_ioctl

#include ADD_CURRENT_DEV


/***********************************/
/********** i94xxx_pll_dev ********/
#define DT_DEV_NAME				i94xxx_pll_clk_dev
#define DT_DEV_MODULE			clk_cntl

#define CLK_DT_IOCTL			clock_i94xxx_pll_ioctl

#include ADD_CURRENT_DEV


/***********************************/
/********** i94xxx_hclk_dev ********/
#define DT_DEV_NAME				i94xxx_hclk_clk_dev
#define DT_DEV_MODULE			clk_cntl

#define CLK_DT_IOCTL			clock_i94xxx_hclk_ioctl

#include ADD_CURRENT_DEV


/***********************************/
/********** i94xxx_pclk0_dev ********/
#define DT_DEV_NAME				i94xxx_pclk0_clk_dev
#define DT_DEV_MODULE			clk_cntl

#define CLK_DT_IOCTL			clock_i94xxx_pclk0_ioctl

#include ADD_CURRENT_DEV


/***********************************/
/********** i94xxx_pclk1_dev ********/
#define DT_DEV_NAME				i94xxx_pclk1_clk_dev
#define DT_DEV_MODULE			clk_cntl

#define CLK_DT_IOCTL			clock_i94xxx_pclk1_ioctl

#include ADD_CURRENT_DEV



/***********************************/
/********** i94xxx_spi2clk_dev ********/
#define DT_DEV_NAME				i94xxx_spi2clk_clk_dev
#define DT_DEV_MODULE			clk_cntl

#define CLK_DT_IOCTL			clock_i94xxx_spi2clk_ioctl

#include ADD_CURRENT_DEV



/***********************************/
/********** i94xxx_spi2clk_dev ********/
#define DT_DEV_NAME				i94xxx_uart0clk_clk_dev
#define DT_DEV_MODULE			clk_cntl

#define CLK_DT_IOCTL			clock_i94xxx_uart0clk_ioctl

#include ADD_CURRENT_DEV


/***********************************/
/********** i94xxx_i2s_clk_dev ********/
#define DT_DEV_NAME				i94xxx_i2s_clk_dev
#define DT_DEV_MODULE			clk_cntl

#define CLK_DT_IOCTL			clock_i94xxx_i2s_ioctl

#include ADD_CURRENT_DEV


/***********************************/
/********** i94xxx_dpwm_clk_dev ********/
#define DT_DEV_NAME				i94xxx_dpwm_clk_dev
#define DT_DEV_MODULE			clk_cntl

#define CLK_DT_IOCTL			clock_i94xxx_dpwm_ioctl

#include ADD_CURRENT_DEV


/***********************************/
/********** i94xxx_i2c1_clk_dev ********/
#define DT_DEV_NAME				i94xxx_i2c1_clk_dev
#define DT_DEV_MODULE			clk_cntl

#define CLK_DT_IOCTL			clock_i94xxx_i2c1_ioctl

#include ADD_CURRENT_DEV


/***********************************/
/********** i94xxx_spi0_clk_dev ********/
#define DT_DEV_NAME				i94xxx_spi0_clk_dev
#define DT_DEV_MODULE			clk_cntl

#define CLK_DT_IOCTL			clock_i94xxx_spi0_ioctl

#include ADD_CURRENT_DEV


/***********************************/
/********** i94xxx_spi2clk_dev ********/
#define DT_DEV_NAME				i94xxx_systick_clk_dev
#define DT_DEV_MODULE			clk_cntl

#define CLK_DT_IOCTL			i94xxx_systick_clk_ioctl
#define CLK_DT_DEFAULT_PARENT	i94xxx_hclk_clk_dev

#include ADD_CURRENT_DEV

