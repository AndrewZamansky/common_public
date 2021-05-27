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




static void get_parent_clock_rate(struct cfg_clk_t *cfg_clk, uint32_t *rate)
{
	struct dev_desc_t * parent_clk_dev;

	parent_clk_dev = cfg_clk->parent_clk;
	if (NULL == parent_clk_dev) CRITICAL_ERROR("bad parent clock\n");
	DEV_IOCTL_1_PARAMS(parent_clk_dev, CLK_IOCTL_GET_FREQ, rate);
}


uint8_t clock_xtensa_sim_core_clk_ioctl( struct dev_desc_t *adev,
		const uint8_t aIoctl_num, void * aIoctl_param1,
		void * aIoctl_param2)
{
	struct cfg_clk_t *cfg_clk;

	cfg_clk = DEV_GET_CONFIG_DATA_POINTER(adev);
	switch(aIoctl_num)
	{
	case CLK_IOCTL_SET_FREQ :
		return 1;
		break;
	case CLK_IOCTL_GET_FREQ :
		*(uint32_t*)aIoctl_param1 = cfg_clk->rate;
		break;
	default :
		return 1;
	}
	return 0;
}


/**
 * clock_control_xtensa_sim_ioctl()
 *
 * return:
 */
uint8_t clock_control_xtensa_sim_ioctl( struct dev_desc_t *adev,
		const uint8_t aIoctl_num, void * aIoctl_param1,
		void * aIoctl_param2)
{
//	struct clk_cntl_xtensa_sim_cfg_t *cfg_hndl;
//	uint32_t rate;
//	struct dev_desc_t	*parent_clk_dev;
//
//	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(adev);

	switch(aIoctl_num)
	{
	case IOCTL_DEVICE_START :

		break;


	default :
		return 1;
	}
	return 0;
}
