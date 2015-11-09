/* ****************************************
 *****************************************
	@file   <semihosting.c>

	@date   :
	@project  :
	@description :
***************************************** */

/* ------------------------ INCLUDES ---------------------------------------*/

#include "arm_semihosting_config.h"
#include "dev_managment_api.h" // for device manager defines and typedefs
#include "src/_semihosting_prerequirements_check.h" // should be after {arm_semihosting_config.h,dev_managment_api.h}

#include "ARM_api.h"


/* ------------------------ defines ------------------------------*/

/* ------------------------typedefs ------------------------------*/

/* ---------------------------- External variables ---------------------------------*/

/* ------------------------ External functions ------------------------------*/

/* ------------------------------ Local variables ---------------------------------*/


/* ------------------------ Local Functions  -------------------------------*/


static int BKPT(int op, void* p1, void* p2) ;
int smihosting_is_active=0;
#ifdef ARM_SEMIHOSTING_CONFIG_ENABLE_TX_DONE_CALLBACK
	static pdev_descriptor   callback_dev;
#endif
static int terminal_hndl;



#define SYS_OPEN    	0x01
#define SYS_CLOSE    	0x02
#define SYS_READ    	0x06
#define SYS_READC   	0x07
#define SYS_WRITE		0x05
#define SYS_WRITE0 		0x04
#define SYS_FLEN 		0x0C
#define SYS_TMPNAM 		0x0D



/********  types  *********************/

/********  externals *********************/
extern uint32_t bkpt_asm(int op, void* p1, void* p2);


/********  local defs *********************/




void ARM_API_SH_Write0(const uint8_t* ptr)
{
  BKPT(SYS_WRITE0, (void*) ptr, (void*) 0);
}


/*
 * function ARM_API_SH_Open()
 */
int ARM_API_SH_Open(char* fileName, int mode)
{
  int retVal;
  void* block[3];



  block[0] = (void*)fileName;
  block[1] = (void*)mode;
  block[2] = (void*)strlen(fileName);
  retVal = BKPT(SYS_OPEN, (void*) block, (void*) 0);

  return retVal;
}

/*
 * function ARM_API_SH_Close()
 */
int ARM_API_SH_Close(int FileHandle)
{
  int retVal;
  void* block[3];

  block[0] = (void*)FileHandle;
  retVal = BKPT(SYS_CLOSE, (void*) block, (void*) 0);

  return retVal;
}


/*
 * function ARM_API_SH_Read()
 */
int ARM_API_SH_Read(int FileHandle, uint8_t* pBuffer, int NumBytesToRead)
{
  int NumBytesLeft;
  void* block[3];

  block[0] = (void*)FileHandle;
  block[1] = (void*)pBuffer;
  block[2] = (void*)NumBytesToRead;
  NumBytesLeft = BKPT(SYS_READ, (void*) block, (void*) 0);

  return NumBytesLeft;
}

/*
 * function ARM_API_SH_GetFileLength()
 */
int ARM_API_SH_GetFileLength(int FileHandle)
{
  int fileLength;
  void* block[3];

  block[0] = (void*)FileHandle;

  fileLength = BKPT(SYS_FLEN, (void*) block, (void*) 0);

  return fileLength;
}


char _SH_ReadC(void) {
  char c;

  c = BKPT(SYS_READC, (void*) 0, (void*) 0);
  return c;
}

int ARM_API_SH_Write(int FileHandle, const uint8_t* pBuffer, int NumBytesToWrite)
{
  int NumBytesLeft;
  void* block[3];

  block[0] = (void*)FileHandle;
  block[1] = (void*)pBuffer;
  block[2] = (void*)NumBytesToWrite;
  NumBytesLeft = BKPT(SYS_WRITE, (void*) block, (void*) 0);

  return NumBytesLeft;
}


static int BKPT(int op, void* p1, void* p2)
{
	if(0==smihosting_is_active)
	{
		return -1;
	}
	return bkpt_asm(op, p1, p2);
}



/*
 * get line input from semihost
 */
void arm_get_line_from_console(uint8_t* pBuffer,int maxLen)
{
	int i=0;
	uint8_t curr_char=0;


	while ((i < maxLen) && ('\n' != pBuffer[i]))
	{
		ARM_API_SH_Read(0, &curr_char, 1);
		pBuffer[i]=curr_char;
		i++;
	}

}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        arm_sh_pwrite                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
size_t arm_sh_pwrite(const void *aHandle ,const uint8_t *apData , size_t aLength, size_t aOffset)
{
	ARM_API_SH_Write(terminal_hndl,apData,aLength);
#ifdef ARM_SEMIHOSTING_CONFIG_ENABLE_TX_DONE_CALLBACK
	if(NULL != callback_dev)
		DEV_CALLBACK_1_PARAMS(callback_dev , CALLBACK_TX_DONE,(void*)aLength); // !!! to avoid recursivity transmited length should be '>=aLength'
#endif
	return aLength;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        arm_sh_ioctl                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t arm_sh_ioctl( void * const aHandle ,const uint8_t aIoctl_num , void * aIoctl_param1, void * aIoctl_param2)
{
	switch(aIoctl_num)
	{
		case IOCTL_GET_PARAMS_ARRAY_FUNC :
			*(uint8_t*)aIoctl_param2 =  0; // size
			break;

		case IOCTL_DEVICE_START :
			terminal_hndl=ARM_API_SH_Open(":tt",5);//mode wb

			break;
#ifdef ARM_SEMIHOSTING_CONFIG_ENABLE_TX_DONE_CALLBACK
		case IOCTL_SET_ISR_CALLBACK_DEV:
			callback_dev =(pdev_descriptor) aIoctl_param1;
			break;
#endif
		default :
			return 1;
	}
	return 0;
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ARM_SH_API_Init_Dev_Descriptor                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t  arm_sh_api_init_dev_descriptor(pdev_descriptor aDevDescriptor)
{
	if(NULL == aDevDescriptor) return 1;


	aDevDescriptor->handle = NULL;
	aDevDescriptor->ioctl = arm_sh_ioctl;
	aDevDescriptor->pwrite = arm_sh_pwrite;

	return 0 ;

}

