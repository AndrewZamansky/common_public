
#ifndef _u_boot_shell_API_H_
#define _u_boot_shell_API_H_


#include "src/_u_boot_shell_prerequirements_check.h"

/*****************  defines  **************/

#define U_BOOT_SHELL_API_SERVER_DEVICE_STR "server_dev"
/**********  define API  types ************/





/**********  define API  functions  ************/
uint8_t  u_boot_shell_api_init_dev_descriptor(pdev_descriptor_t aDevDescriptor);

#include "src/_u_boot_shell_static_dev_macros.h"

#define U_BOOT_SHELL_API_CREATE_STATIC_DEV(pdev ,server_pdev ) \
			__U_BOOT_SHELL_API_CREATE_STATIC_DEV(pdev ,server_pdev)



#endif
