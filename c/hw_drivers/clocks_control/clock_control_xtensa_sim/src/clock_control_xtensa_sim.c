/*
 *
 * file :   clocks_control_xtensa_sim.c
 *
 *
 */


#include "_project_typedefs.h"
#include "_project_defines.h"

#include "dev_management_api.h"

#include "clock_control_api.h"
#include "clock_control_xtensa_sim_api.h"
#include "clock_control_xtensa_sim.h"


/*******************************************/
/********** xtensa_sim_core_clk_dev ********/
static void xtensa_sim_core_clk_set_freq(uint32_t freq, uint32_t parent_freq)
{
	// do nothing
}

#define DT_DEV_NAME                xtensa_sim_core_clk_dev
#define DT_DEV_MODULE              clk_cntl

#define CLK_DT_SET_FREQ_FUNC      xtensa_sim_core_clk_set_freq

#include "clk_cntl_add_device.h"



/**
 * clock_control_xtensa_sim_ioctl()
 *
 * return:
 */
uint8_t clock_control_xtensa_sim_ioctl( struct dev_desc_t *adev,
		const uint8_t aIoctl_num, void * aIoctl_param1,
		void * aIoctl_param2)
{
	struct clock_control_xtensa_sim_cfg_t *cfg_hndl;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(clock_control_xtensa_sim, adev);


	switch(aIoctl_num)
	{
	case IOCTL_DEVICE_START :
		DEV_IOCTL(xtensa_sim_core_clk_dev, IOCTL_DEVICE_START);
		DEV_IOCTL(xtensa_sim_core_clk_dev,
						CLK_IOCTL_SET_FREQ, &cfg_hndl->sim_core_rate);
		break;

	default :
		return 1;
	}
	return 0;
}

#define MODULE_NAME                  clock_control_xtensa_sim
#define MODULE_IOCTL_FUNCTION        clock_control_xtensa_sim_ioctl
#define MODULE_HAS_NO_CONFIG_DATA
#define MODULE_HAS_NO_RUNTIME_DATA
#include "add_module.h"
