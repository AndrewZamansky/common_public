
#ifndef _Shell_API_H_
#define _Shell_API_H_

#ifdef CONFIG_INCLUDE_SHELL

#include "src/_shell_prerequirements_check.h"

/*****************  defines  **************/


/**********  define API  types ************/



#define SHELL_API_SERVER_DEVICE_STR  	"server_device"


/**********  define API  functions  ************/
uint8_t  shell_api_init_dev_descriptor(pdev_descriptor aDevDescriptor);

#include "src/shell_static_dev_macros.h"

#define SHELL_API_CREATE_STATIC_DEV(dev,dev_name ,server_dev ) \
			__SHELL_API_CREATE_STATIC_DEV(dev,dev_name ,server_dev)

#else

#include "dev_managment_api.h"

#endif

extern pdev_descriptor_const gCurrReplyDev;

#include "PRINTF_api.h"
#if !defined(__CSMC__)// cosmic compiler fix
#define SHELL_REPLY_PRINTF(...) 	PRINTF_REPLY(gCurrReplyDev , __VA_ARGS__)
#endif
#define SHELL_REPLY_STR(str) 		PRINT_STR_REPLY(gCurrReplyDev , str)
#define SHELL_REPLY_DATA(data,len) 	PRINT_DATA_REPLY(gCurrReplyDev , data,len)


#endif
