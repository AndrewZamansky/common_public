/*
 *
 *   file  :  pwr_stm32f10x.c
 *
 */

#include "stm32f10x_pwr.h"
#include "pwr_stm32f10x_api.h"
#include "pwr_stm32f10x.h"
#include "clock_control_stm32f10x_api.h"


static void init_pwr(struct pwr_stm32f10x_runtime_t *runtime_handle)
{
	DEV_IOCTL_0_PARAMS(stm32f10x_pwr_clk_dev, IOCTL_DEVICE_START);
	DEV_IOCTL_0_PARAMS(stm32f10x_pwr_clk_dev, CLK_IOCTL_ENABLE);

	runtime_handle->init_done = 1;
}



/**
 * pwr_stm32f10x_ioctl()
 *
 * return:
 */
uint8_t pwr_stm32f10x_ioctl( struct dev_desc_t *adev,
		const uint8_t aIoctl_num, void *aIoctl_param1, void *aIoctl_param2)
{
	struct pwr_stm32f10x_runtime_t *runtime_handle;

	runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(pwr_stm32f10x, adev);
	if ((0 == runtime_handle->init_done) && (IOCTL_DEVICE_START != aIoctl_num))
	{
		CRITICAL_ERROR("not initialized yet");
	}

	switch(aIoctl_num)
	{
	case IOCTL_DEVICE_START :
		init_pwr(runtime_handle);
		break;
	}
	return 0;
}

#define MODULE_NAME                     pwr_stm32f10x
#define MODULE_IOCTL_FUNCTION           pwr_stm32f10x_ioctl
#define MODULE_HAS_NO_CONFIG_DATA
#define MODULE_HAS_NO_RUNTIME_DATA

#include "add_module.h"
