
#ifndef _u_boot_shell_API_H_
#define _u_boot_shell_API_H_


#include "src/_u_boot_shell_prerequirements_check.h"

/*****************  defines  **************/

#define U_BOOT_SHELL_API_SERVER_DEVICE_STR "server_dev"
/**********  define API  types ************/




/**********  define API  functions  ************/

extern struct dev_desc_t * gCurrReplyDev;

#include "PRINTF_api.h"
#if !defined(__CSMC__)// cosmic compiler fix
#define SHELL_REPLY_PRINTF(...)  	 PRINTF_REPLY(gCurrReplyDev, __VA_ARGS__)
#endif
#define SHELL_REPLY_STR(str) 	  	 PRINT_STR_REPLY(gCurrReplyDev, (char*)str)
#define SHELL_REPLY_DATA(data,len) 	 PRINT_DATA_REPLY(gCurrReplyDev, data, len)

#endif
