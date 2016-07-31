/*
 *
 *   file  :  GPIO_STM32F10x.c
 *
 */



/***************   includes    *******************/
#include "src/_gpio_stm32f10x_prerequirements_check.h"

#include "stm32f10x_gpio.h"
#include "gpio_api.h"
#include "gpio_stm32f10x_api.h"
#include "gpio_stm32f10x.h"




/***************   defines    *******************/
#define GPIO_STM32F10x_MAX_NUM_OF_GPIOS 4



/***************   typedefs    *******************/

/**********   external variables    **************/



/***********   local variables    **************/

static GPIO_TypeDef  *ports[]={GPIOA,GPIOB,GPIOC};


/*
 * function : GPIO_STM32F10x_Init()
 *
 *
 */
uint8_t GPIO_STM32F10x_Init(GPIO_STM32F10X_Instance_t *pInstance)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_TypeDef* GPIOx = ports[pInstance->port_num];
	
	GPIO_InitStructure.GPIO_Pin =   pInstance->pin_num_mask  ;
	GPIO_InitStructure.GPIO_Mode = pInstance->mode ;


	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init( GPIOx, &GPIO_InitStructure );


	return 0;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        gpio_stm32f10x_ioctl                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t gpio_stm32f10x_ioctl( pdev_descriptor_t apdev ,const uint8_t aIoctl_num
		, void * aIoctl_param1 , void * aIoctl_param2)
{
	GPIO_STM32F10X_Instance_t *handle;
	GPIO_TypeDef* GPIOx;
	uint32_t pin_num_mask;

	handle = apdev->handle;
	GPIOx = ports[handle->port_num];
	pin_num_mask = handle->pin_num_mask;

	switch(aIoctl_num)
	{
		case IOCTL_GPIO_STM32F10X_SET_PORT_PARAM :
			{
				handle->port_num = ((char*)aIoctl_param1)[0]-'a';
			}
			break;
		case IOCTL_GPIO_STM32F10X_SET_PIN_PARAM :
			{
				uint8_t pin_num;
				pin_num = atoi((char*)aIoctl_param1);
				handle->pin_num_mask = 1 << pin_num;
			}
			break;
		case IOCTL_GPIO_STM32F10X_SET_MODE_PARAM :
			if (0 == memcmp((uint8_t*) aIoctl_param1 , "output" , sizeof("output") ))
			{
				handle->mode = GPIO_Mode_Out_PP;
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
			GPIO_STM32F10x_Init(handle);
			break;

		case IOCTL_GPIO_PIN_SET :
			GPIO_SetBits(GPIOx,  pin_num_mask );

			break;

		case IOCTL_GPIO_PIN_CLEAR :
			GPIO_ResetBits(GPIOx,   pin_num_mask );
			break;

		case IOCTL_GPIO_PIN_READ :
			*((uint8_t*)aIoctl_param1) = GPIO_ReadInputDataBit( GPIOx,   pin_num_mask );
			break;

		default :
			return 1;
	}
	return 0;
}
