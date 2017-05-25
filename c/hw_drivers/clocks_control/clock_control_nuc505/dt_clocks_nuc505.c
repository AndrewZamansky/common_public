#include "_project.h"

#include "dev_management_api.h"


/***********************************/
/********** nuc505_xtal_clk_dev ********/
#define DT_DEV_NAME				nuc505_xtal_clk_dev
#define DT_DEV_MODULE			clk_cntl

#define CLK_DT_IOCTL			clock_nuc505_xtal_ioctl
#define CLK_DT_DEFAULT_RATE		CONFIG_CRYSTAL_CLOCK

#include ADD_CURRENT_DEV


/***********************************/
/********** nuc505_core_clk_dev ********/
#define DT_DEV_NAME				nuc505_core_clk_dev
#define DT_DEV_MODULE			clk_cntl

#define CLK_DT_IOCTL			clock_nuc505_core_ioctl

#include ADD_CURRENT_DEV


/***********************************/
/********** nuc505_spi2clk_dev ********/
#define DT_DEV_NAME				nuc505_systick_clk_dev
#define DT_DEV_MODULE			clk_cntl

#define CLK_DT_IOCTL			nuc505_systick_clk_ioctl
#define CLK_DT_DEFAULT_PARENT	nuc505_core_clk_dev

#include ADD_CURRENT_DEV

