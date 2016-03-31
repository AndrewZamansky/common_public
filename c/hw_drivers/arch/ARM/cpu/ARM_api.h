
#ifndef _ARM_API_H_
#define _ARM_API_H_

#include "arm_semihosting_config.h"
#include "dev_managment_api.h" // for device manager defines and typedefs
#include "src/_semihosting_prerequirements_check.h" // should be after {arm_semihosting_config.h,dev_managment_api.h}


/**********  defines ************/


#define SYSTEM_CLOCK

/**********  define API  types ************/


/**********  define API  functions  ************/

//int ARM_API_Init(void);
//int ARM_API_SH_Init(void);
int ARM_API_SH_Open(char* fileName, int mode);
int ARM_API_SH_Close(int FileHandle);
int ARM_API_SH_Read(int FileHandle, uint8_t * pBuffer, int NumBytesToRead);
int ARM_API_SH_GetFileLength(int FileHandle);
int ARM_API_SH_Write(int FileHandle, const uint8_t* pBuffer, int NumBytesToWrite) ;
void ARM_API_SH_Write0(const uint8_t* ptr);

uint8_t  arm_sh_api_init_dev_descriptor(pdev_descriptor aDevDescriptor);

#include "src/semihosting_static_dev_macros.h"

#define ARM_SH_API_CREATE_STATIC_DEV(dev,dev_name,callback_dev) \
				__ARM_SH_API_CREATE_STATIC_DEV(dev,dev_name,callback_dev)

#endif