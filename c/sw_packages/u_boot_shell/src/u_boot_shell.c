/*
 *
 * file :   u_boot_shell.c
 *
 *
 *
 *
 *
 */



/********  includes *********************/
#include "src/_u_boot_shell_prerequirements_check.h"

#include "u_boot_shell_api.h"

#include "u_boot_shell.h"
#include "config.h"

#ifdef  _BOOTSTAGE_H
	typedef enclosure_unnedded _code_by_DONT_USE_STD_IO_in_common_h_in_uboor_include_dir  dummy_type;
#endif


/********  defines *********************/

#define INSTANCE(hndl)	((u_boot_shell_instance_t*)hndl)


/********  types  *********************/



/********  externals *********************/

/* ------------------------------ Exported variables ---------------------------------*/
pdev_descriptor_const gCurrReplyDev;

extern int run_command(const char *cmd, int flag);

/********  local defs *********************/
#if CONFIG_U_BOOT_SHELL_MAX_NUM_OF_DYNAMIC_INSTANCES > 0


static const dev_param_t u_boot_shell_Dev_Params[]=
{
		{IOCTL_SET_SERVER_DEVICE_BY_NAME , IOCTL_VOID , (uint8_t*)U_BOOT_SHELL_API_SERVER_DEVICE_STR, NOT_FOR_SAVE},
};

u_boot_shell_instance_t	u_boot_shell_instances[CONFIG_U_BOOT_SHELL_MAX_NUM_OF_DYNAMIC_INSTANCES] = {{0}};
uint8_t usedUbootShellInstances = 0;

#endif



char console_buffer[1];

void clear_ctrlc (void)
{

}

int had_ctrlc (void)
{
	return 0;
}

int ctrlc()
{
	return 0;
}

ulong get_timer(ulong base)
{
	return 0;
}

int cli_readline(const char *const prompt)
{
	return 0;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        u_boot_shell_callback                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t u_boot_shell_callback(void * const aHandle ,const uint8_t aCallback_num
		, void * aCallback_param1, void * aCallback_param2)
{

	char *pCmdStart;
	if(CALLBACK_DATA_RECEIVED == aCallback_num)
	{
		pCmdStart =( char *)aCallback_param1;
		pCmdStart[((size_t)aCallback_param2) - 1]='\0';
		run_command((const char *)pCmdStart,0);
	}
	return 0;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:       u_boot_shell_ioctl                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t u_boot_shell_ioctl( void * const aHandle ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2)
{

	switch(aIoctl_num)
	{
		case IOCTL_GET_PARAMS_ARRAY_FUNC :
#if CONFIG_U_BOOT_SHELL_MAX_NUM_OF_DYNAMIC_INSTANCES > 0
			*(const dev_param_t**)aIoctl_param1  = u_boot_shell_Dev_Params;
			*(uint8_t*)aIoctl_param2 =  sizeof(u_boot_shell_Dev_Params)/sizeof(dev_param_t); //size
#else
			*(uint8_t*)aIoctl_param2 =  0; //size
#endif
			break;

#if CONFIG_U_BOOT_SHELL_MAX_NUM_OF_DYNAMIC_INSTANCES > 0
		case IOCTL_SET_SERVER_DEVICE_BY_NAME :
			{
				pdev_descriptor_t server_device;
				server_device = DEV_OPEN((uint8_t*)aIoctl_param1);
				if(NULL != server_device)
				{
					DEV_IOCTL(server_device, IOCTL_SET_ISR_CALLBACK_DEV ,  (void*)INSTANCE(aHandle)->this_dev);
				}

				INSTANCE(aHandle)->server_dev=server_device;
				gCurrReplyDev = server_device;
			}
			break;
#endif
		case IOCTL_DEVICE_START :

			break;

		default :
			return 1;
	}
	return 0;
}

#if CONFIG_U_BOOT_SHELL_MAX_NUM_OF_DYNAMIC_INSTANCES > 0

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        Shell_API_Init_Dev_Descriptor                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t  u_boot_shell_api_init_dev_descriptor(pdev_descriptor_t aDevDescriptor)
{
	u_boot_shell_instance_t *pInstance;

	if(CONFIG_U_BOOT_SHELL_MAX_NUM_OF_DYNAMIC_INSTANCES <= usedUbootShellInstances) return 1;

	if(NULL == aDevDescriptor) return 1;

	pInstance = &u_boot_shell_instances[usedUbootShellInstances];

	aDevDescriptor->handle = pInstance;
	pInstance->this_dev = aDevDescriptor;

	aDevDescriptor->ioctl = u_boot_shell_ioctl;
	aDevDescriptor->callback = u_boot_shell_callback;

	usedUbootShellInstances++;

	return 0;
}

#endif

