#include "_project.h"

#include "dev_management_api.h"


/***********************************/
/********** stm32f10x_xtal_clk_dev ********/
#define DT_DEV_NAME                stm32f10x_xtal_clk_dev
#define DT_DEV_MODULE              clk_cntl

#define CLK_DT_IOCTL               clock_stm32f10x_xtal_ioctl

#include ADD_CURRENT_DEV



/***********************************/
/********** stm32f10x_hsirc_clk_dev ********/
#define DT_DEV_NAME               stm32f10x_hsirc_clk_dev
#define DT_DEV_MODULE             clk_cntl

#define CLK_DT_IOCTL              clock_stm32f10x_hsirc_ioctl

#include ADD_CURRENT_DEV


/***********************************/
/********** stm32f10x_sysclk_dev ********/
#define DT_DEV_NAME               stm32f10x_sysclk_dev
#define DT_DEV_MODULE             clk_cntl

#define CLK_DT_IOCTL              clock_stm32f10x_sysclk_ioctl

#include ADD_CURRENT_DEV



/***********************************/
/********** stm32f10x_hclk_dev ********/
#define DT_DEV_NAME               stm32f10x_hclk_clk_dev
#define DT_DEV_MODULE             clk_cntl

#define CLK_DT_IOCTL              clock_stm32f10x_hclk_ioctl

#include ADD_CURRENT_DEV


/***********************************/
/********** stm32f10x_spi2clk_dev ********/
#define DT_DEV_NAME                stm32f10x_systick_clk_dev
#define DT_DEV_MODULE              clk_cntl

#define CLK_DT_IOCTL               stm32f10x_systick_clk_ioctl
#define CLK_DT_DEFAULT_PARENT      stm32f10x_hclk_clk_dev

#include ADD_CURRENT_DEV


