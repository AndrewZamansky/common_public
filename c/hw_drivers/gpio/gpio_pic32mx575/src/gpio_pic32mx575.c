/*
 *
 *   file  :  GPIO_PIC32MX575.c
 *
 */

#include "gpio_api.h"
#include "gpio_pic32mx575_api.h"
#include "gpio_pic32mx575.h"

#include "gpio_pic32mx575_add_component.h"


uint8_t gpio_pic32mx575_ioctl( struct dev_desc_t *adev , const uint8_t aIoctl_num
		, void * aIoctl_param1 , void * aIoctl_param2)
{
	struct gpio_pic32mx575_instance_t *config_handle;

	config_handle = DEV_GET_CONFIG_DATA_POINTER(adev);

	switch(aIoctl_num)
	{
		case IOCTL_GPIO_PIC32MX575_SET_PACKAGE_PIN_PARAM :
			{
				config_handle->package_pin_number = atoi((char*)aIoctl_param1);
			}
			break;
		case IOCTL_GPIO_PIC32MX575_SET_MODE_PARAM :
			if (0 == memcmp((uint8_t*) aIoctl_param1 , "output" , sizeof("output") ))
			{
				config_handle->mode = GPIO_PIC32MX575_API_MODE_OUT;
			}
			else // other modes will be added later
			{
//				switch(pInstance->mode)
//				{
//					case GPIO_API_IN_FLOATING:
//						GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//						break;
//					case GPIO_API_IN_PULL_DOWN:
//						GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
//						break;
//					case GPIO_API_IN_PULL_UP:
//						GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
//						break;
//					case GPIO_API_OUT:
//						GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//						break;
//					default :
//						return 1;
//				}
				return 1;
			}
			break;

		case IOCTL_DEVICE_START :
			if (GPIO_PIC32MX575_API_MODE_IN == config_handle->mode)
			{
				*(uint32_t*)config_handle->port_base_address |= config_handle->pin_number_mask;
			}
			else
			{
				*(uint32_t*)config_handle->port_base_address &= ~(config_handle->pin_number_mask);
			}

			break;

		case IOCTL_GPIO_PIN_SET :
			*(uint32_t*)(config_handle->port_base_address + 0x20) |= config_handle->pin_number_mask;

			break;

		case IOCTL_GPIO_PIN_CLEAR :
			*(uint32_t*)(config_handle->port_base_address + 0x20) &= ~(config_handle->pin_number_mask);
			break;

		case IOCTL_GPIO_PIN_READ :
			//*((uint8_t*)aIoctl_param1) = MEM_ADDR(GPIOx + 0x0C) &   pin_num_mask ;
			break;

		default :
			return 1;
	}
	return 0;
}
