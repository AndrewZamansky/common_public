/*
 *
 *   file  :  GPIO_STM8.c
 *
 */



/***************   includes    *******************/
#include "src/_gpio_stm8_prerequirements_check.h"
#include "gpio_api.h"

#include "gpio_stm8_api.h"
#include "gpio_stm8.h"

#include "stm8s.h"

#include "uart_stm8_add_component.h"

/***************   defines    *******************/




/***************   typedefs    *******************/

/**********   external variables    **************/



/***********   local variables    **************/


#define INSTANCE(hndl)	((GPIO_STM8_Instance_t*)hndl)



/*
 * function : GPIO_STM8_Init()
 *
 *
 */
uint8_t GPIO_STM8_Init(GPIO_STM8_Instance_t *pInstance)
{
    GPIO_Init(pInstance->GPIOx , pInstance->pin_num, pInstance->mode);

	return 0;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        gpio_stm8_ioctl                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t gpio_stm8_ioctl( pdev_descriptor_t apdev ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2)
{
	uint8_t pin_num;
	GPIO_TypeDef* GPIOx;

	pin_num = INSTANCE(aHandle)->pin_num;
	GPIOx = INSTANCE(aHandle)->GPIOx;

	switch(aIoctl_num)
	{
#if UART_STM8_CONFIG_NUM_OF_DYNAMIC_INSTANCES>0
		case IOCTL_GPIO_STM8_SET_PORT_PARAM :
			{
				uint8_t port_num = ((char*)aIoctl_param1)[0]-'a';
				INSTANCE(aHandle)->GPIOx = ports[port_num];
			}
			break;
		case IOCTL_GPIO_STM8_SET_PIN_PARAM :
			pin_num = atoi((char*)aIoctl_param1);
			INSTANCE(aHandle)->pin_num = 1<<pin_num;
			break;
		case IOCTL_GPIO_STM8_SET_MODE_PARAM :
			if (0 == memcmp((uint8_t*) aIoctl_param1 , "output" , sizeof("output") ))
			{
				INSTANCE(aHandle)->mode = GPIO_STM8_AP_MODE_OUT_PP_LOW_FAST;
			}
			else
			{
				return 1;
			}
			break;
#endif
		case IOCTL_DEVICE_START :
			GPIO_STM8_Init(INSTANCE(aHandle));
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
