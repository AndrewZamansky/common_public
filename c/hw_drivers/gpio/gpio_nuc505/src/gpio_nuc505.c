/*
 *
 *   file  :  GPIO_NUC505.c
 *
 */



/***************   includes    *******************/
#include "_project_typedefs.h"
#include "_project_defines.h"

#include "dev_management_api.h"

#include "gpio_api.h"
#include "gpio_nuc505_api.h"
#include "gpio_nuc505.h"

#include "NUC505Series.h"
#include "gpio.h"

#include "_gpio_nuc505_prerequirements_check.h"

/*following line add module to available module list for dynamic device tree*/
#include "gpio_nuc505_add_component.h"


struct GPIO_PX_t {
    __IO uint32_t MODE;
    __IO uint32_t PUEN;
    __IO uint32_t DOUT;
    __I  uint32_t PIN;
};



/**
 * gpio_nuc505_ioctl()
 *
 * return:
 */
uint8_t gpio_nuc505_ioctl( struct dev_desc_t *adev , const uint8_t aIoctl_num
		, void * aIoctl_param1 , void * aIoctl_param2)
{
	struct gpio_nuc505_cfg_t *config_handle;
	struct struct GPIO_PX_t* GPIOx;
	uint32_t volatile * pDOUT;
	uint16_t pin_num_mask;

	config_handle = DEV_GET_CONFIG_DATA_POINTER(adev);
	GPIOx = (struct GPIO_PX_t*)config_handle->port_num;
	pin_num_mask = config_handle->pin_num_mask;
	pDOUT = &GPIOx->DOUT;

	switch(aIoctl_num)
	{
#ifdef CONFIG_USE_RUNTIME_DEVICE_CONFIGURATION
		case IOCTL_GPIO_NUC505_SET_PORT_PARAM :
			config_handle->port_num =  *(uint32_t*)aIoctl_param1 ;
			break;
		case IOCTL_GPIO_NUC505_SET_SINGLE_PIN_NUMBER_PARAM :
			{
				uint8_t pin_num;
				pin_num = *(uint8_t*)aIoctl_param1 ;
				config_handle->pin_num_mask = 1 << pin_num;
			}
			break;
		case IOCTL_GPIO_NUC505_SET_MODE_PARAM :
			config_handle->mode = *(uint32_t*)aIoctl_param1 ;
			break;
#endif

		case IOCTL_DEVICE_START :
			GPIO_SetMode(GPIOx , pin_num_mask  , config_handle->mode);
			break;

		case IOCTL_GPIO_PIN_SET :
			*pDOUT |= pin_num_mask;

			break;

		case IOCTL_GPIO_PIN_CLEAR :
			*pDOUT &= (~pin_num_mask);
			break;

		case IOCTL_GPIO_PIN_READ :
			*((uint8_t*)aIoctl_param1) = GPIOx->PIN &  pin_num_mask ;
			break;

		default :
			return 1;
	}
	return 0;
}
