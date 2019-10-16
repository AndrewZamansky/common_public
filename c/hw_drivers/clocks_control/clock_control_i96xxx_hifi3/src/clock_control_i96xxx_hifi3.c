/*
 *
 * file :   clocks_control_i96xxx_hifi3.c
 *
 *
 */



/********  includes *********************/
#include "_project_typedefs.h"
#include "_project_defines.h"

#include "dev_management_api.h"

#include "clock_control_api.h"
#include "clock_control_i96xxx_hifi3_api.h"
#include "clock_control_common_api.h"
#include "clock_control_i96xxx_hifi3.h"



/********  defines *********************/

/********  types  *********************/


/********  externals *********************/


/********  local variables *********************/


static void clock_i96xxx_hifi3_dsp_set_freq(uint32_t freq, uint32_t parent_freq)
{

}

#define DT_DEV_NAME                i96xxx_hifi3_dsp_clk_dev
#define DT_DEV_MODULE              clk_cntl

#define CLK_DT_SET_FREQ_FUNC      clock_i96xxx_hifi3_dsp_set_freq

#include "clk_cntl_add_device.h"




static void init_clocks(struct clk_cntl_i96xxx_hifi3_cfg_t *cfg_hndl)
{
	uint32_t rate;

	DEV_IOCTL_0_PARAMS(i96xxx_hifi3_dsp_clk_dev, IOCTL_DEVICE_START);
	DEV_IOCTL_1_PARAMS(i96xxx_hifi3_dsp_clk_dev,
			CLK_IOCTL_SET_FREQ, &cfg_hndl->dsp_rate);

}


/**
 * clock_control_i96xxx_hifi3_ioctl()
 *
 * return:
 */
static uint8_t clock_control_i96xxx_hifi3_ioctl( struct dev_desc_t *adev,
		const uint8_t aIoctl_num, void * aIoctl_param1,
		void * aIoctl_param2)
{
	struct clk_cntl_i96xxx_hifi3_cfg_t *cfg_hndl;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(adev);

	switch(aIoctl_num)
	{
	case IOCTL_DEVICE_START :
		init_clocks(cfg_hndl);
		break;
	default :
		return 1;
	}
	return 0;
}

#define	MODULE_NAME						clock_control_i96xxx_hifi3
#define	MODULE_IOCTL_FUNCTION			clock_control_i96xxx_hifi3_ioctl
#define MODULE_CONFIG_DATA_STRUCT_TYPE	struct clk_cntl_i96xxx_hifi3_cfg_t
#include "add_module.h"
