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
#include "u_boot_shell_add_component.h"

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
		pCmdStart[((size_t)aCallback_param2) ]='\0';
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
	pdev_descriptor_const   server_dev ;

	switch(aIoctl_num)
	{
#ifdef CONFIG_UBOOT_SHELL_USE_RUNTIME_CONFIGURATION
		case IOCTL_SET_SERVER_DEVICE_BY_NAME :
			{
				server_dev = DEV_OPEN((uint8_t*)aIoctl_param1);
				if(NULL != server_dev)
				{
					DEV_IOCTL(server_dev, IOCTL_SET_ISR_CALLBACK_DEV ,  (void*)INSTANCE(aHandle)->this_dev);
				}

				INSTANCE(aHandle)->server_dev=server_dev;
			}
			break;
#endif
		case IOCTL_DEVICE_START :
			server_dev = INSTANCE(aHandle)->server_dev;
			DEV_IOCTL_0_PARAMS(server_dev , IOCTL_DEVICE_START );

			break;

		default :
			return 1;
	}
	return 0;
}
