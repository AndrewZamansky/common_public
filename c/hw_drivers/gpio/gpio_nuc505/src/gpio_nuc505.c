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



/***************   defines    *******************/
#define GPIO_NUC505_MAX_NUM_OF_GPIOS 4



/***************   typedefs    *******************/

/**********   external variables    **************/



/***********   local variables    **************/
static GPIO_NUC505_Instance_t GPIO_NUC505_InstanceParams[GPIO_NUC505_MAX_NUM_OF_GPIOS];
static uint16_t usedInstances =0 ;
static void  *ports[]={PA,PB,PC,PD};
//static void  *ports[]={NULL,NULL,NULL,NULL};

static const dev_param_t GPIO_NUC505_Dev_Params[]=
{
		{IOCTL_GPIO_NUC505_SET_PORT_PARAM , IOCTL_VOID , (uint8_t*)GPIO_NUC505_API_PORT_STR, NOT_FOR_SAVE},
		{IOCTL_GPIO_NUC505_SET_PIN_PARAM , IOCTL_VOID , (uint8_t*)GPIO_NUC505_API_PIN_STR, NOT_FOR_SAVE},
		{IOCTL_GPIO_NUC505_SET_MODE_PARAM , IOCTL_VOID , (uint8_t*)GPIO_NUC505_API_MODE_STR, NOT_FOR_SAVE},
};

#define INSTANCE(hndl)	((GPIO_NUC505_Instance_t*)hndl)



/*
 * function : GPIO_NUC505_Init()
 *
 *
 */
uint8_t GPIO_NUC505_Init(GPIO_NUC505_Instance_t *pInstance)
{
//	void* GPIOx = ports[pInstance->port_num];


	GPIO_SetMode(pInstance->port_num, pInstance->pin_num_mask  , pInstance->mode);

	return 0;
}


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
uint8_t gpio_nuc505_ioctl( void * const aHandle ,const uint8_t aIoctl_num
		, void * aIoctl_param1 , void * aIoctl_param2)
{
	void* GPIOx;
	uint32_t pin_num_mask;

	GPIOx = INSTANCE(aHandle)->port_num;
	pin_num_mask = INSTANCE(aHandle)->pin_num_mask;

	switch(aIoctl_num)
	{
		case IOCTL_GET_PARAMS_ARRAY_FUNC :
			*(const dev_param_t**)aIoctl_param1  = GPIO_NUC505_Dev_Params;
			*(uint8_t*)aIoctl_param2 =  sizeof(GPIO_NUC505_Dev_Params)/sizeof(dev_param_t); //size
			break;

		case IOCTL_GPIO_NUC505_SET_PORT_PARAM :
			{
				INSTANCE(aHandle)->port_num = ports[((char*)aIoctl_param1)[0]-'a'];
			}
			break;
		case IOCTL_GPIO_NUC505_SET_PIN_PARAM :
			{
				uint8_t pin_num;
				pin_num = atoi((char*)aIoctl_param1);
				INSTANCE(aHandle)->pin_num_mask = 1 << pin_num;
			}
			break;
		case IOCTL_GPIO_NUC505_SET_MODE_PARAM :
			if (0 == memcmp((uint8_t*) aIoctl_param1 , "output" , sizeof("output") ))
			{
				INSTANCE(aHandle)->mode = GPIO_MODE_OUTPUT;
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
			GPIO_NUC505_Init(INSTANCE(aHandle));
			break;

		case IOCTL_GPIO_PIN_SET :
			MEM_ADDR(GPIOx + 0x08) = pin_num_mask;

			break;

		case IOCTL_GPIO_PIN_CLEAR :
			MEM_ADDR(GPIOx + 0x08) = MEM_ADDR(GPIOx + 0x08) & (~pin_num_mask);
			break;

		case IOCTL_GPIO_PIN_READ :
			*((uint8_t*)aIoctl_param1) = MEM_ADDR(GPIOx + 0x0C) &   pin_num_mask ;
			break;

		default :
			return 1;
	}
	return 0;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        gpio_nuc505_api_dev_descriptor                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t  gpio_nuc505_api_init_dev_descriptor(pdev_descriptor_t aDevDescriptor)
{
	if(NULL == aDevDescriptor) return 1;
	if (usedInstances >= GPIO_NUC505_MAX_NUM_OF_GPIOS) return 1;


	aDevDescriptor->handle = &GPIO_NUC505_InstanceParams[usedInstances];
	aDevDescriptor->ioctl = gpio_nuc505_ioctl;
	usedInstances++;

	return 0 ;

}

