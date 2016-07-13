/*
 *
 *   file  :  GPIO_REMOTE.c
 *
 */



/***************   includes    *******************/
#include "_project_typedefs.h"
#include "_project_defines.h"
#include "_project_func_declarations.h"

#include "dev_management_api.h" // for device manager defines and typedefs

#include "gpio_api.h"
#include "remote_devices_api.h"
#include "gpio_remote_api.h"




/***************   defines    *******************/
#define GPIO_REMOTE_MAX_NUM_OF_GPIOS 4

#define GPIO_REMOTE_OUT 1

/***************   typedefs    *******************/
typedef struct {
	uint8_t addr_num[16];
	uint8_t pin_num;
	uint8_t mode;
	pdev_descriptor_t server_dev;
} GPIO_REMOTE_Instance_t;


/**********   external variables    **************/



/***********   local variables    **************/
static GPIO_REMOTE_Instance_t GPIO_REMOTE_InstanceParams[GPIO_REMOTE_MAX_NUM_OF_GPIOS];
static uint16_t usedInstances =0 ;

static const dev_param_t GPIO_REMOTE_Dev_Params[]=
{
		{IOCTL_SET_SERVER_DEVICE_BY_NAME , IOCTL_VOID , (uint8_t*)GPIO_REMOTE_API_SERVER_DEVICE_STR, NOT_FOR_SAVE},
		{IOCTL_GPIO_REMOTE_SET_ADDR_PARAM , IOCTL_VOID , (uint8_t*)GPIO_REMOTE_API_ADDR_STR, NOT_FOR_SAVE},
		{IOCTL_GPIO_REMOTE_SET_PIN_PARAM , IOCTL_VOID , (uint8_t*)GPIO_REMOTE_API_PIN_STR, NOT_FOR_SAVE},
		{IOCTL_GPIO_REMOTE_SET_MODE_PARAM , IOCTL_VOID , (uint8_t*)GPIO_REMOTE_API_MODE_STR, NOT_FOR_SAVE},
};

#define INSTANCE(hndl)	((GPIO_REMOTE_Instance_t*)hndl)



/*
 * function : GPIO_REMOTE_Init()
 *
 *
 */
uint8_t GPIO_REMOTE_Init(GPIO_REMOTE_Instance_t *pInstance)
{


	return 0;
}

static uint8_t cmd[4];//=(uint8_t*)"axx\n";
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        gpio_remote_ioctl                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t gpio_remote_ioctl( void * const aHandle ,const uint8_t aIoctl_num
		, void * aIoctl_param1 , void * aIoctl_param2)
{
	pdev_descriptor_t server_dev;
	uint8_t i,pin_num;
	remote_cmd_t	remote_cmd;
	char str_num[3];
	remote_cmd.addr_num = INSTANCE(aHandle)->addr_num;
	pin_num = INSTANCE(aHandle)->pin_num;
	server_dev = INSTANCE(aHandle)->server_dev;

	cmd[3]='\n';
	remote_cmd.comand_str_len = 4;

	switch(aIoctl_num)
	{
		case IOCTL_GET_PARAMS_ARRAY_FUNC :
			*(const dev_param_t**)aIoctl_param1  = GPIO_REMOTE_Dev_Params;
			*(uint8_t*)aIoctl_param2 = sizeof(GPIO_REMOTE_Dev_Params)/sizeof(dev_param_t); //size
			break;

		case IOCTL_SET_SERVER_DEVICE_BY_NAME :
			INSTANCE(aHandle)->server_dev = DEV_OPEN((uint8_t*)aIoctl_param1);
			break;

		case IOCTL_GPIO_REMOTE_SET_ADDR_PARAM :
			str_num[2]=0;
			for(i=0;i<16;i++)
			{
				memcpy(str_num,&((char*)aIoctl_param1)[i*2],2);
				INSTANCE(aHandle)->addr_num[i]=strtol(str_num,NULL,16);
			}
			break;
		case IOCTL_GPIO_REMOTE_SET_PIN_PARAM :
			pin_num = atoi((char*)aIoctl_param1);
			INSTANCE(aHandle)->pin_num = pin_num;
			break;
		case IOCTL_GPIO_REMOTE_SET_MODE_PARAM :
			if (0 == memcmp((uint8_t*) aIoctl_param1 , "output" , sizeof("output") ))
			{
				INSTANCE(aHandle)->mode = GPIO_REMOTE_OUT;
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
			GPIO_REMOTE_Init(INSTANCE(aHandle));
			break;

		case IOCTL_GPIO_PIN_SET :
			cmd[0]='a';
			cmd[1]='0'+INSTANCE(aHandle)->pin_num;
			cmd[2]='1';
			remote_cmd.comand_str = cmd;
			DEV_IOCTL(server_dev,IOCTL_REMOTE_SEND_CMD, &remote_cmd);

			break;

		case IOCTL_GPIO_PIN_CLEAR :
			cmd[0]='a';
			cmd[1]='0'+INSTANCE(aHandle)->pin_num;
			cmd[2]='0';
			remote_cmd.comand_str = cmd;
			DEV_IOCTL(server_dev,IOCTL_REMOTE_SEND_CMD, &remote_cmd);
			break;

		case IOCTL_GPIO_PIN_READ :
			cmd[0]='b';
			cmd[1]='0'+INSTANCE(aHandle)->pin_num;
			cmd[2]='0';
			remote_cmd.comand_str = cmd;
			DEV_IOCTL(server_dev,IOCTL_REMOTE_SEND_CMD, &remote_cmd);
			*((uint8_t*)aIoctl_param1) = remote_cmd.result[0];
			break;

		default :
			return 1;
	}
	return 0;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        gpio_remote_api_dev_descriptor                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t  gpio_remote_api_init_dev_descriptor(pdev_descriptor_t aDevDescriptor)
{
	if(NULL == aDevDescriptor) return 1;
	if (usedInstances >= GPIO_REMOTE_MAX_NUM_OF_GPIOS) return 1;


	aDevDescriptor->handle = &GPIO_REMOTE_InstanceParams[usedInstances];
	aDevDescriptor->ioctl = gpio_remote_ioctl;
	usedInstances++;

	return 0 ;

}

