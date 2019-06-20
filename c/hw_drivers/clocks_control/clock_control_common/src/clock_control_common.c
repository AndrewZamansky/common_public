/*
 *
 * file :   clocks_control_common.c
 *
 *
 */



/********  includes *********************/
#include "_project_typedefs.h"
#include "_project_defines.h"

#include "dev_management_api.h"

#include "clock_control_api.h"

#include "clock_control_common_api.h"


/********  defines *********************/

/********  types  *********************/


/********  externals *********************/


/********  local variables *********************/


static void get_root_clock(struct dev_desc_t *adev,
								struct dev_desc_t **root_clk_dev)
{
	struct cfg_clk_t *cfg_clk;

	cfg_clk = DEV_GET_CONFIG_DATA_POINTER(adev);
	while (NULL != cfg_clk->parent_clk)
	{
		adev = cfg_clk->parent_clk;
		cfg_clk = DEV_GET_CONFIG_DATA_POINTER(adev);
	}
	*root_clk_dev = adev;
}


void clock_control_common_api_get_parent_clock_rate(
								struct cfg_clk_t *cfg_clk, uint32_t *rate)
{
	struct dev_desc_t * parent_clk_dev;

	parent_clk_dev = cfg_clk->parent_clk;
	if (NULL == parent_clk_dev) CRITICAL_ERROR("bad parent clock\n");
	DEV_IOCTL_1_PARAMS(parent_clk_dev, CLK_IOCTL_GET_FREQ, rate);
}


uint8_t common_clock_api_standard_clk_ioctls(struct dev_desc_t *adev,
		const uint8_t aIoctl_num, void * aIoctl_param1, void * aIoctl_param2)
{
	struct cfg_clk_t *cfg_clk;

	cfg_clk = DEV_GET_CONFIG_DATA_POINTER(adev);

	switch(aIoctl_num)
	{
	case CLK_IOCTL_GET_FREQ :
		clock_control_common_api_get_parent_clock_rate(cfg_clk, aIoctl_param1);
		break;
	case CLK_IOCTL_GET_ROOT_CLK:
		get_root_clock(adev, aIoctl_param1);
		break;
	default :
		return 1;
	}
	return 0;
}
