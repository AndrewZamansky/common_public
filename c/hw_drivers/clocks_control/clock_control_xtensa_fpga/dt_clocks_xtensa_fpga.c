#include "_project.h"

#include "dev_management_api.h"


/***********************************/
/********** xtensa_sim_core_clk_dev ********/
#define DT_DEV_NAME				xtensa_fpga_core_clk_dev
#define DT_DEV_MODULE			clk_cntl

#define CLK_DT_IOCTL			clock_xtensa_fpga_core_clk_ioctl
#define CLK_DT_DEFAULT_RATE		CONFIG_XTENSA_FPGA_SIM_CORE_CLOCK

#include ADD_CURRENT_DEV
