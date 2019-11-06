
#ifndef _semihosting_api_H_
#define _semihosting_api_H_

#include "_project_typedefs.h"
#include "_project_defines.h"
#include "dev_common_ioctl_and_types.h"
#include "dev_management_api.h"

#define SYSTEM_CLOCK

enum ARM_ioctl_t {
	IOCTL_ARM_SH_CALL_NO_OS_TASK = IOCTL_LAST_COMMON_IOCTL+1
};


struct semihosting_instance_t {
	struct dev_desc_t *   callback_dev;
};
SET_CONFIG_TYPE(semihosting, struct semihosting_instance_t);

//int ARM_API_Init(void);
//int ARM_API_SH_Init(void);
int ARM_API_SH_Open(const char* fileName, size_t mode);
int ARM_API_SH_Close(int FileHandle);
int ARM_API_SH_Read(int FileHandle, uint8_t * pBuffer, size_t NumBytesToRead);
int ARM_API_SH_GetFileLength(int FileHandle);
int ARM_API_SH_Write(
		int FileHandle, const uint8_t* pBuffer, size_t NumBytesToWrite) ;
void ARM_API_SH_Write0(const uint8_t* ptr);


#endif
