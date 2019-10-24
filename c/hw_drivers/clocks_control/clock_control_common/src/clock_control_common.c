/*
 *
 * file :   clocks_control_common.c
 *
 *
 */

#include "_project_typedefs.h"
#include "_project_defines.h"

#include "dev_management_api.h"

#include "clock_control_api.h"

#include "clock_control_common_api.h"
#include "clock_control_common.h"


#define MODULE_NAME                      clk_cntl


static void get_root_clock(struct dev_desc_t *adev,
								struct dev_desc_t **root_clk_dev)
{
	struct runtime_clk_t *runtime_data;

	runtime_data = DEV_GET_RUNTIME_DATA_POINTER(MODULE_NAME, adev);

	while (NULL != runtime_data->parent_clk)
	{
		adev = runtime_data->parent_clk;
		runtime_data = DEV_GET_RUNTIME_DATA_POINTER(MODULE_NAME, adev);
	}
	*root_clk_dev = adev;
}


static void get_rate(struct cfg_clk_t *cfg_clk,
		struct runtime_clk_t *runtime_data, struct dev_desc_t  *parent_clk,
		uint32_t *rate)
{
	uint32_t parent_rate;

	parent_rate = 0;
	if (NULL != parent_clk)
	{
		DEV_IOCTL_1_PARAMS(parent_clk, CLK_IOCTL_GET_FREQ, &parent_rate);
	}
	if (NULL != cfg_clk->get_freq_func)
	{
		cfg_clk->get_freq_func(rate, parent_rate);
	}
	else
	{
		if (0 == runtime_data->rate)
		{
			if (NULL == parent_clk)
			{
				CRITICAL_ERROR("parent clock was not set");
			}
			*rate = parent_rate;
		}
		else
		{
			*rate = runtime_data->rate;
		}
	}
}


static uint8_t clk_cntl_ioctl(struct dev_desc_t *adev,
		const uint8_t aIoctl_num, void * aIoctl_param1, void * aIoctl_param2)
{
	struct cfg_clk_t *cfg_clk;
	struct runtime_clk_t *runtime_data;
	struct dev_desc_t  *parent_clk;
	uint32_t parent_rate;

	cfg_clk = DEV_GET_CONFIG_DATA_POINTER(MODULE_NAME, adev);
	runtime_data = DEV_GET_RUNTIME_DATA_POINTER(MODULE_NAME, adev);
	parent_clk = runtime_data->parent_clk;
	parent_rate = 0;

	if ((IOCTL_DEVICE_START != aIoctl_num) && (0 == runtime_data->init_done))
	{
		CRITICAL_ERROR("clock not initialized yet\n");
	}

	switch(aIoctl_num)
	{
	case IOCTL_DEVICE_START :
		runtime_data->parent_clk = cfg_clk->default_parent_clk;
		runtime_data->init_done = 1;
		break;
	case CLK_IOCTL_GET_FREQ :
		get_rate(cfg_clk, runtime_data, parent_clk, aIoctl_param1);
		break;
	case CLK_IOCTL_SET_FREQ :
		if (NULL != cfg_clk->set_freq_func)
		{
			if (NULL != parent_clk)
			{
				DEV_IOCTL_1_PARAMS(
						parent_clk, CLK_IOCTL_GET_FREQ, &parent_rate);
			}
			cfg_clk->set_freq_func(*(uint32_t*)aIoctl_param1, parent_rate);
			runtime_data->rate = *(uint32_t*)aIoctl_param1;
		}
		break;
	case CLK_IOCTL_SET_PARENT :
		if (NULL != cfg_clk->set_parent_clk_func)
		{
			cfg_clk->set_parent_clk_func(aIoctl_param1);
			runtime_data->parent_clk = aIoctl_param1;
		}
		break;
	case CLK_IOCTL_ENABLE:
		if (NULL != cfg_clk->enable_clk_func)
		{
			cfg_clk->enable_clk_func();
		}
		break;
	case CLK_IOCTL_GET_ROOT_CLK:
		get_root_clock(adev, aIoctl_param1);
		break;
	default :
		return 1;
	}
	return 0;
}

#define MODULE_IOCTL_FUNCTION            clk_cntl_ioctl
#include "add_module.h"
