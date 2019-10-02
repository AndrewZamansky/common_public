#include "_project.h"

#include "dev_management_api.h"


/***********************************/
/********** stm32f10x_xtal_clk_dev ********/
#define DT_DEV_NAME                stm32f10x_xtal_clk_dev
#define DT_DEV_MODULE              clk_cntl

#define CLK_DT_IOCTL               clock_stm32f10x_xtal_ioctl

#include"../clk_cntl_add_device.h"



/***********************************/
/********** stm32f10x_hsirc_clk_dev ********/
#define DT_DEV_NAME               stm32f10x_hsirc_clk_dev
#define DT_DEV_MODULE             clk_cntl

#define CLK_DT_IOCTL              clock_stm32f10x_hsirc_ioctl

#include"../clk_cntl_add_device.h"


/***********************************/
/********** stm32f10x_sysclk_dev ********/
#define DT_DEV_NAME               stm32f10x_sysclk_dev
#define DT_DEV_MODULE             clk_cntl

#define CLK_DT_IOCTL              clock_stm32f10x_sysclk_ioctl

#include"../clk_cntl_add_device.h"



/***********************************/
/********** stm32f10x_hclk_dev ********/
#define DT_DEV_NAME               stm32f10x_hclk_clk_dev
#define DT_DEV_MODULE             clk_cntl

#define CLK_DT_IOCTL              clock_stm32f10x_hclk_ioctl
#define CLK_DT_DEFAULT_PARENT      stm32f10x_sysclk_dev

#include"../clk_cntl_add_device.h"


/***********************************/
/********** stm32f10x_systick_dev ********/
#define DT_DEV_NAME                stm32f10x_systick_clk_dev
#define DT_DEV_MODULE              clk_cntl

#define CLK_DT_IOCTL               stm32f10x_systick_clk_ioctl
#define CLK_DT_DEFAULT_PARENT      stm32f10x_hclk_clk_dev

#include"../clk_cntl_add_device.h"


/***********************************/
/********** stm32f10x_apb2_clk_dev ********/
#define DT_DEV_NAME               stm32f10x_apb2_clk_dev
#define DT_DEV_MODULE             clk_cntl

#define CLK_DT_IOCTL              clock_stm32f10x_apb2_ioctl
#define CLK_DT_DEFAULT_PARENT      stm32f10x_hclk_clk_dev

#include"../clk_cntl_add_device.h"


/***********************************/
/********** stm32f10x_adc_clk_dev ********/
#define DT_DEV_NAME               stm32f10x_adc_clk_dev
#define DT_DEV_MODULE             clk_cntl

#define CLK_DT_IOCTL              clock_stm32f10x_adc_ioctl
#define CLK_DT_DEFAULT_PARENT      stm32f10x_apb2_clk_dev

#include"../clk_cntl_add_device.h"
