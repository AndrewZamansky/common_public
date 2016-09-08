/*
 *
 *   file  :  GPIO_NUC505.c
 *
 */



/***************   includes    *******************/
#include "src/_gpio_nuc505_prerequirements_check.h"

#include "gpio_api.h"
#include "gpio_nuc505_api.h"
#include "gpio_nuc505.h"

#include "NUC505Series.h"
#include "gpio.h"

#include "gpio_nuc505_add_component.h"

/***************   defines    *******************/



/***************   typedefs    *******************/

typedef struct
{
    __IO uint32_t MODE;
    __IO uint32_t PUEN;
    __IO uint32_t DOUT;
    __I  uint32_t PIN;
} GPIO_PX_T;

/**********   external variables    **************/



/***********   local variables    **************/
static void  *ports[]={PA,PB,PC,PD};
//static void  *ports[]={NULL,NULL,NULL,NULL};




/*---------------------------------------------------------------------------------------------------------*/
/* Function:        gpio_nuc505_ioctl                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t gpio_nuc505_ioctl( pdev_descriptor_t apdev , const uint8_t aIoctl_num
		, void * aIoctl_param1 , void * aIoctl_param2)
{
	gpio_nuc505_instance_t *config_handle;
	GPIO_PX_T* GPIOx;
	uint32_t volatile * pDOUT;
	uint16_t pin_num_mask;

	config_handle = DEV_GET_CONFIG_DATA_POINTER(apdev);
	GPIOx = (GPIO_PX_T*)config_handle->port_num;
	pin_num_mask = config_handle->pin_num_mask;
	pDOUT = &GPIOx->DOUT;

	switch(aIoctl_num)
	{
		case IOCTL_GPIO_NUC505_SET_PORT_PARAM :
			{
				config_handle->port_num = (uint32_t)ports[((char*)aIoctl_param1)[0]-'a'];
			}
			break;

		case IOCTL_GPIO_NUC505_SET_PIN_PARAM :
			{
				uint8_t pin_num;
				pin_num = atoi((char*)aIoctl_param1);
				config_handle->pin_num_mask = 1 << pin_num;
			}
			break;

		case IOCTL_GPIO_NUC505_SET_MODE_PARAM :
			if (0 == memcmp((uint8_t*) aIoctl_param1 , "output" , sizeof("output") ))
			{
				config_handle->mode = GPIO_MODE_OUTPUT;
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


