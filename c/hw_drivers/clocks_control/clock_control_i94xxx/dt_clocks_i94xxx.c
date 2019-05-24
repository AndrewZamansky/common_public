#include "_project.h"

#include "dev_management_api.h"


/***********************************/
/********** i94xxx_xtal_clk_dev ********/
#define DT_DEV_NAME                i94xxx_xtal_clk_dev
#define DT_DEV_MODULE              clk_cntl

#define CLK_DT_IOCTL               clock_i94xxx_xtal_ioctl

#include ADD_CURRENT_DEV


/***********************************/
/********** i94xxx_lxtal_clk_dev ********/
#define DT_DEV_NAME                i94xxx_lxtal_clk_dev
#define DT_DEV_MODULE              clk_cntl

#define CLK_DT_IOCTL               clock_i94xxx_lxtal_ioctl

#include ADD_CURRENT_DEV


/***********************************/
/********** i94xxx_hirc_clk_dev ********/
#define DT_DEV_NAME               i94xxx_hirc_clk_dev
#define DT_DEV_MODULE             clk_cntl

#define CLK_DT_IOCTL              clock_i94xxx_hirc_ioctl

#include ADD_CURRENT_DEV


/***********************************/
/********** i94xxx_pll_dev ********/
#define DT_DEV_NAME               i94xxx_pll_clk_dev
#define DT_DEV_MODULE             clk_cntl

#define CLK_DT_IOCTL              clock_i94xxx_pll_ioctl

#include ADD_CURRENT_DEV


/***********************************/
/********** i94xxx_hclk_dev ********/
#define DT_DEV_NAME               i94xxx_hclk_clk_dev
#define DT_DEV_MODULE             clk_cntl

#define CLK_DT_IOCTL              clock_i94xxx_hclk_ioctl

#include ADD_CURRENT_DEV


/***********************************/
/********** i94xxx_pclk0_dev ********/
#define DT_DEV_NAME               i94xxx_pclk0_clk_dev
#define DT_DEV_MODULE             clk_cntl

#define CLK_DT_IOCTL              clock_i94xxx_pclk0_ioctl

#include ADD_CURRENT_DEV


/***********************************/
/********** i94xxx_pclk1_dev ********/
#define DT_DEV_NAME               i94xxx_pclk1_clk_dev
#define DT_DEV_MODULE             clk_cntl

#define CLK_DT_IOCTL              clock_i94xxx_pclk1_ioctl

#include ADD_CURRENT_DEV



/***********************************/
/********** i94xxx_spi2clk_dev ********/
#define DT_DEV_NAME               i94xxx_spi2clk_clk_dev
#define DT_DEV_MODULE             clk_cntl

#define CLK_DT_IOCTL              clock_i94xxx_spi2clk_ioctl

#include ADD_CURRENT_DEV


/***********************************/
/********** i94xxx_spi1clk_dev ********/
#define DT_DEV_NAME				i94xxx_spi1clk_clk_dev
#define DT_DEV_MODULE			clk_cntl

#define CLK_DT_IOCTL			clock_i94xxx_spi1clk_ioctl

#include ADD_CURRENT_DEV


/***********************************/
/********** i94xxx_spi2clk_dev ********/
#define DT_DEV_NAME               i94xxx_uart0clk_clk_dev
#define DT_DEV_MODULE             clk_cntl

#define CLK_DT_IOCTL              clock_i94xxx_uart0clk_ioctl

#include ADD_CURRENT_DEV


/***********************************/
/********** i94xxx_i2s_clk_dev ********/
#define DT_DEV_NAME               i94xxx_i2s_clk_dev
#define DT_DEV_MODULE             clk_cntl

#define CLK_DT_IOCTL              clock_i94xxx_i2s_ioctl

#include ADD_CURRENT_DEV


/***********************************/
/********** i94xxx_dpwm_clk_dev ********/
#define DT_DEV_NAME                i94xxx_dpwm_clk_dev
#define DT_DEV_MODULE              clk_cntl

#define CLK_DT_IOCTL               clock_i94xxx_dpwm_ioctl

#include ADD_CURRENT_DEV

/***********************************/
/********** i94xxx_i2c0_clk_dev ********/
#define DT_DEV_NAME                i94xxx_i2c0_clk_dev
#define DT_DEV_MODULE              clk_cntl

#define CLK_DT_IOCTL               clock_i94xxx_i2c0_ioctl

#include ADD_CURRENT_DEV

/***********************************/
/********** i94xxx_i2c1_clk_dev ********/
#define DT_DEV_NAME                i94xxx_i2c1_clk_dev
#define DT_DEV_MODULE              clk_cntl

#define CLK_DT_IOCTL               clock_i94xxx_i2c1_ioctl

#include ADD_CURRENT_DEV


/***********************************/
/********** i94xxx_spi0_clk_dev ********/
#define DT_DEV_NAME                i94xxx_spi0_clk_dev
#define DT_DEV_MODULE              clk_cntl

#define CLK_DT_IOCTL               clock_i94xxx_spi0_ioctl

#include ADD_CURRENT_DEV


/***********************************/
/********** i94xxx_usb_clk_dev ********/
#define DT_DEV_NAME               i94xxx_usb_clk_dev
#define DT_DEV_MODULE             clk_cntl

#define CLK_DT_IOCTL              clock_i94xxx_usb_ioctl

#include ADD_CURRENT_DEV


/***********************************/
/********** i94xxx_spi2clk_dev ********/
#define DT_DEV_NAME                i94xxx_systick_clk_dev
#define DT_DEV_MODULE              clk_cntl

#define CLK_DT_IOCTL               i94xxx_systick_clk_ioctl
#define CLK_DT_DEFAULT_PARENT      i94xxx_hclk_clk_dev

#include ADD_CURRENT_DEV


/***********************************/
/********** i94xxx_I2S_MCLK_clk_dev ********/
#define DT_DEV_NAME                i94xxx_I2S_MCLK_clk_dev
#define DT_DEV_MODULE              clk_cntl

#define CLK_DT_IOCTL               i94xxx_I2S_MCLK_ioctl
#define CLK_DT_DEFAULT_PARENT      i94xxx_i2s_clk_dev

#include ADD_CURRENT_DEV


/***********************************/
/********** i94xxx_I2S_BCLK_clk_dev ********/
#define DT_DEV_NAME                i94xxx_I2S_BCLK_clk_dev
#define DT_DEV_MODULE              clk_cntl

#define CLK_DT_IOCTL               i94xxx_I2S_BCLK_ioctl
#define CLK_DT_DEFAULT_PARENT      i94xxx_i2s_clk_dev

#include ADD_CURRENT_DEV


/***********************************/
/********** i94xxx_I2S_FSCLK_clk_dev ********/
#define DT_DEV_NAME                i94xxx_I2S_FSCLK_clk_dev
#define DT_DEV_MODULE              clk_cntl

#define CLK_DT_IOCTL               i94xxx_I2S_FSCLK_ioctl
#define CLK_DT_DEFAULT_PARENT      i94xxx_I2S_BCLK_clk_dev

#include ADD_CURRENT_DEV

/***********************************/
/********** i94xxx_I2S_onSPI1_MCLK_clk_dev ********/
#define DT_DEV_NAME                i94xxx_I2S_onSPI1_MCLK_clk_dev
#define DT_DEV_MODULE              clk_cntl

#define CLK_DT_IOCTL               i94xxx_I2S_onSPI1_MCLK_ioctl
#define CLK_DT_DEFAULT_PARENT      i94xxx_spi1clk_clk_dev

#include ADD_CURRENT_DEV


/***********************************/
/********** i94xxx_I2S_onSPI1_BCLK_clk_dev ********/
#define DT_DEV_NAME                i94xxx_I2S_onSPI1_BCLK_clk_dev
#define DT_DEV_MODULE              clk_cntl

#define CLK_DT_IOCTL               i94xxx_I2S_onSPI1_BCLK_ioctl
#define CLK_DT_DEFAULT_PARENT      i94xxx_spi1clk_clk_dev

#include ADD_CURRENT_DEV


/***********************************/
/********** i94xxx_I2S_FSCLK_onSPI1_clk_dev ********/
#define DT_DEV_NAME                i94xxx_I2S_onSPI1_FSCLK_clk_dev
#define DT_DEV_MODULE              clk_cntl

#define CLK_DT_IOCTL               i94xxx_I2S_onSPI1_FSCLK_ioctl
#define CLK_DT_DEFAULT_PARENT      i94xxx_I2S_onSPI1_BCLK_clk_dev

#include ADD_CURRENT_DEV


/***********************************/
/********** i94xxx_I2S_onSPI2_MCLK_clk_dev ********/
#define DT_DEV_NAME                i94xxx_I2S_onSPI2_MCLK_clk_dev
#define DT_DEV_MODULE              clk_cntl

#define CLK_DT_IOCTL               i94xxx_I2S_onSPI2_MCLK_ioctl
#define CLK_DT_DEFAULT_PARENT      i94xxx_spi2clk_clk_dev

#include ADD_CURRENT_DEV


/***********************************/
/********** i94xxx_I2S_onSPI2_BCLK_clk_dev ********/
#define DT_DEV_NAME                i94xxx_I2S_onSPI2_BCLK_clk_dev
#define DT_DEV_MODULE              clk_cntl

#define CLK_DT_IOCTL               i94xxx_I2S_onSPI2_BCLK_ioctl
#define CLK_DT_DEFAULT_PARENT      i94xxx_spi2clk_clk_dev

#include ADD_CURRENT_DEV


/***********************************/
/********** i94xxx_I2S_FSCLK_onSPI2_clk_dev ********/
#define DT_DEV_NAME                i94xxx_I2S_onSPI2_FSCLK_clk_dev
#define DT_DEV_MODULE              clk_cntl

#define CLK_DT_IOCTL               i94xxx_I2S_onSPI2_FSCLK_ioctl
#define CLK_DT_DEFAULT_PARENT      i94xxx_I2S_onSPI2_BCLK_clk_dev

#include ADD_CURRENT_DEV


/***********************************/
/********** i94xxx_dmic_clk_dev ********/
#define DT_DEV_NAME                i94xxx_dmic_clk_dev
#define DT_DEV_MODULE              clk_cntl

#define CLK_DT_IOCTL               clock_i94xxx_dmic_ioctl

#include ADD_CURRENT_DEV


