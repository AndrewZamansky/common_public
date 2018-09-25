/*
 *
 *   file  :  GPIO_I94XXX.c
 *
 */



/***************   includes    *******************/
#include "_project_typedefs.h"
#include "_project_defines.h"

#include "dev_management_api.h"

#include "I94100.h"
#include "gpio.h"

#include "gpio_i94xxx_api.h"
#include "gpio_i94xxx.h"

#include "gpio_api.h"
#include "irq_api.h"

#include "_gpio_i94xxx_prerequirements_check.h"

/*following line add module to available module list for dynamic device tree*/
#include "gpio_i94xxx_add_component.h"


/***************   defines    *******************/



/***************   typedefs    *******************/


/**********   external variables    **************/



/***********   local variables    **************/


uint8_t gpio_i94xxx_callback(struct dev_desc_t *adev ,
		uint8_t aCallback_num , void * aCallback_param1,
		void * aCallback_param2)
{
	struct dev_desc_t *client_dev;
	struct gpio_i94xxx_config_t *config_handle;
	GPIO_T *GPIOx;
	uint16_t pin_num_mask;

	config_handle = DEV_GET_CONFIG_DATA_POINTER(adev);

	GPIOx = (GPIO_T *)config_handle->port_num;
	pin_num_mask = config_handle->pin_num_mask;
	GPIO_CLR_INT_FLAG(GPIOx, pin_num_mask);
	client_dev = (struct dev_desc_t *)config_handle->client_dev;
	if(NULL == client_dev)
	{
		return 1;
	}
	DEV_CALLBACK_0_PARAMS(client_dev, CALLBACK_DATA_RECEIVED);

	return 0;
}

/**
 * gpio_configure_pinout()
 *
 * return:
 */
uint8_t gpio_configure_pinout(GPIO_T *GPIOx, uint16_t pin_num_mask,
								struct gpio_i94xxx_config_t *config_handle)
{

	// SMTEN - Schmitt Trigger Enable. This is to further prevent bouncing
	//			and signals that may be too low or high.
	switch(pin_num_mask)
	{
	case GPIO_I94XXX_API_PIN_0 :
		config_handle->pin_num = 0;
		GPIOx->SMTEN = GPIO_SMTEN_SMTEN0_Msk;
		break;

	case GPIO_I94XXX_API_PIN_1 :
		config_handle->pin_num = 1;
		GPIOx->SMTEN = GPIO_SMTEN_SMTEN1_Msk;
		break;

	case GPIO_I94XXX_API_PIN_2 :
		config_handle->pin_num = 2;
		GPIOx->SMTEN = GPIO_SMTEN_SMTEN2_Msk;
		break;

	case GPIO_I94XXX_API_PIN_3 :
		config_handle->pin_num = 3;
		GPIOx->SMTEN = GPIO_SMTEN_SMTEN3_Msk;
		break;

	case GPIO_I94XXX_API_PIN_4 :
		config_handle->pin_num = 4;
		GPIOx->SMTEN = GPIO_SMTEN_SMTEN4_Msk;
		break;

	case GPIO_I94XXX_API_PIN_5 :
		config_handle->pin_num = 5;
		GPIOx->SMTEN = GPIO_SMTEN_SMTEN5_Msk;
		break;

	case GPIO_I94XXX_API_PIN_6 :
		config_handle->pin_num = 6;
		GPIOx->SMTEN = GPIO_SMTEN_SMTEN6_Msk;
		break;

	case GPIO_I94XXX_API_PIN_7 :
		config_handle->pin_num = 7;
		GPIOx->SMTEN = GPIO_SMTEN_SMTEN7_Msk;
		break;

	case GPIO_I94XXX_API_PIN_8 :
		config_handle->pin_num = 8;
		GPIOx->SMTEN = GPIO_SMTEN_SMTEN8_Msk;
		break;

	case GPIO_I94XXX_API_PIN_9 :
		config_handle->pin_num = 9;
		GPIOx->SMTEN = GPIO_SMTEN_SMTEN9_Msk;
		break;

	case GPIO_I94XXX_API_PIN_10 :
		config_handle->pin_num = 10;
		GPIOx->SMTEN = GPIO_SMTEN_SMTEN10_Msk;
		break;

	case GPIO_I94XXX_API_PIN_11 :
		config_handle->pin_num = 11;
		GPIOx->SMTEN = GPIO_SMTEN_SMTEN11_Msk;
		break;

	case GPIO_I94XXX_API_PIN_12 :
		config_handle->pin_num = 12;
		GPIOx->SMTEN = GPIO_SMTEN_SMTEN12_Msk;
		break;

	case GPIO_I94XXX_API_PIN_13 :
		config_handle->pin_num = 13;
		GPIOx->SMTEN = GPIO_SMTEN_SMTEN13_Msk;
		break;

	case GPIO_I94XXX_API_PIN_14 :
		config_handle->pin_num = 14;
		GPIOx->SMTEN = GPIO_SMTEN_SMTEN14_Msk;
		break;

	case GPIO_I94XXX_API_PIN_15 :
		config_handle->pin_num = 15;
		GPIOx->SMTEN = GPIO_SMTEN_SMTEN15_Msk;
		break;

	default:
		CRITICAL_ERROR("GPIO Pin not set");
		break;

	}
	return 0;
}

uint8_t gpio_i94xxx_register_interrupt(struct dev_desc_t *adev,
										struct dev_desc_t *client_dev)
{
	struct gpio_i94xxx_config_t *config_handle;
	GPIO_T* GPIOx;
	IRQn_Type IRQn;

	if(NULL == adev)
	{
		CRITICAL_ERROR("Device not specified");
	}

	config_handle = DEV_GET_CONFIG_DATA_POINTER(adev);
	GPIOx = (GPIO_T*)config_handle->port_num;

	if(NULL != client_dev)
	{
		config_handle->client_dev = client_dev;
	}

	//Set the required IRQn so that button will trigger interrupt
	if(PA == GPIOx)
	{
		IRQn = GPA_IRQn;
	}
	else if(PB == GPIOx)
	{
		IRQn = GPB_IRQn;
	}
	else if(PC == GPIOx)
	{
		IRQn = GPC_IRQn;
	}
	else if(PD == GPIOx)
	{
		IRQn = GPD_IRQn;
	}
	else
	{
		CRITICAL_ERROR("GPIO Port not set");
	}
	//Use this function to make adev available during callback function
	irq_register_device_on_interrupt(IRQn, adev);
	irq_set_priority(IRQn , INTERRUPT_LOWEST_PRIORITY - 1 );
	irq_enable_interrupt(IRQn);

	//Current Button Manager needs to be set for both edges.
	GPIO_EnableInt(GPIOx, config_handle->pin_num, GPIO_INT_BOTH_EDGE);

	return 0;
}

/**
 * gpio_i94xxx_ioctl()
 *
 * return:
 */
uint8_t gpio_i94xxx_ioctl( struct dev_desc_t *adev, const uint8_t aIoctl_num,
		void * aIoctl_param1, void * aIoctl_param2)
{
	struct gpio_i94xxx_config_t *config_handle;
	GPIO_T* GPIOx;

	uint32_t volatile * pDOUT;
	uint16_t pin_num_mask;

	config_handle = DEV_GET_CONFIG_DATA_POINTER(adev);
	GPIOx = (GPIO_T*)config_handle->port_num;
	pin_num_mask = config_handle->pin_num_mask;
	pDOUT = &GPIOx->DOUT;

	switch(aIoctl_num)
	{
#ifdef CONFIG_USE_RUNTIME_DEVICE_CONFIGURATION
	case IOCTL_GPIO_I94XXX_SET_PORT_PARAM :
		config_handle->port_num =  *(uint32_t*)aIoctl_param1 ;
		break;
	case IOCTL_GPIO_I94XXX_SET_SINGLE_PIN_NUMBER_PARAM :
		{
			uint8_t pin_num;
			pin_num = *(uint8_t*)aIoctl_param1 ;
			config_handle->pin_num_mask = 1 << pin_num;
		}
		break;
	case IOCTL_GPIO_I94XXX_SET_MODE_PARAM :
		config_handle->mode = *(uint32_t*)aIoctl_param1 ;
		break;
#endif
	case IOCTL_ADD_ISR_CALLBACK_DEV:

		//Register interrupt and client device.
		gpio_i94xxx_register_interrupt(adev, (struct dev_desc_t *)aIoctl_param1);

		break;

	case IOCTL_DEVICE_START :
		GPIO_SetMode(GPIOx, pin_num_mask, config_handle->mode);
		gpio_configure_pinout(GPIOx, pin_num_mask, config_handle);

		GPIO_DisableInt(GPIOx, config_handle->pin_num);
		if(NULL != config_handle->client_dev)
		{
			gpio_i94xxx_register_interrupt(adev, NULL);
		}

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
