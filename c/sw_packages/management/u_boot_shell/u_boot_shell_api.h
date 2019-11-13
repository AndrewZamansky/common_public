#ifndef _u_boot_shell_API_H_
#define _u_boot_shell_API_H_

#include "project_config.h"

#define UBOOT_SHELL_API_VERSION  20191007

#ifndef CONFIG_INCLUDE_ONLY_UBOOT_SHELL_API
	#include "dev_management_api.h"

	enum U_BOOT_SHELL_API_IOCTL_E
	{
		IOCTL_U_BOOT_SHELL_API_SET_SHELL_FRONTEND = IOCTL_LAST_COMMON_IOCTL + 1,
	};

//	struct u_boot_shell_instance_t {
//		uint8_t dummy;
//	};
//
//	SET_CONFIG_TYPE(u_boot_shell, struct u_boot_shell_instance_t);

#endif

void SHELL_REPLY_PRINTF(const char* Format, ...);
void SHELL_REPLY_STR(const char* str);
void SHELL_REPLY_DATA(uint8_t *data, size_t len);

#endif
