
#ifndef _u_boot_shell_API_H_
#define _u_boot_shell_API_H_

#include "dev_management_api.h"
#include "PRINTF_api.h"


/*****************  defines  **************/

#define  U_BOOT_SHELL_API_SERVER_DEVICE_STR  "server_dev"
/**********  define API  types ************/




/**********  define API  functions  ************/

void SHELL_REPLY_PRINTF(const char* Format, ...);
void SHELL_REPLY_STR(const char* str);
void SHELL_REPLY_DATA(uint8_t *data, size_t len);

#endif
