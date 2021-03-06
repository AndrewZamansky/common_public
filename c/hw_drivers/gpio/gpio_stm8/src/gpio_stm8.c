/*
 *
 *   file  :  GPIO_STM8.c
 *
 */

#include "gpio_api.h"

#include "gpio_stm8_api.h"
#include "gpio_stm8.h"

#include "stm8s.h"

#include "gpio_stm8_add_component.h"


/*
 * function : GPIO_STM8_Init()
 *
 *
 */
uint8_t GPIO_STM8_Init(struct gpio_stm8_instance_t *pInstance)
{
    GPIO_Init(pInstance->GPIOx , pInstance->pin_num, pInstance->mode);

	return 0;
}


uint8_t gpio_stm8_ioctl( struct dev_desc_t *adev ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2)
{
	struct gpio_stm8_instance_t *config_handle;
	uint8_t pin_num;
	GPIO_TypeDef* GPIOx;

	config_handle = DEV_GET_CONFIG_DATA_POINTER(adev);
	pin_num = config_handle->pin_num;
	GPIOx = config_handle->GPIOx;

	switch(aIoctl_num)
	{
#if UART_STM8_CONFIG_NUM_OF_DYNAMIC_INSTANCES>0
		case IOCTL_GPIO_STM8_SET_PORT_PARAM :
			{
				uint8_t port_num = ((char*)aIoctl_param1)[0]-'a';
				config_handle->GPIOx = ports[port_num];
			}
			break;
		case IOCTL_GPIO_STM8_SET_PIN_PARAM :
			pin_num = atoi((char*)aIoctl_param1);
			config_handle->pin_num = 1<<pin_num;
			break;
		case IOCTL_GPIO_STM8_SET_MODE_PARAM :
			if (0 == memcmp((uint8_t*) aIoctl_param1 , "output" , sizeof("output") ))
			{
				config_handle->mode = GPIO_STM8_AP_MODE_OUT_PP_LOW_FAST;
			}
			else
			{
				return 1;
			}
			break;
#endif
		case IOCTL_DEVICE_START :
			GPIO_STM8_Init(config_handle);
			break;

		case IOCTL_GPIO_PIN_SET :
			GPIO_WriteHigh(GPIOx,  pin_num );

			break;

		case IOCTL_GPIO_PIN_CLEAR :
			GPIO_WriteLow(GPIOx,  pin_num );
			break;

		case IOCTL_GPIO_PIN_READ :
			*((uint8_t*)aIoctl_param1) = GPIO_ReadInputPin( GPIOx,  pin_num );
			break;

		default :
			return 1;
	}
	return 0;
}
