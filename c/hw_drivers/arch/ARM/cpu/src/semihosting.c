/* ****************************************
 *****************************************
	@file   <semihosting.c>

	@date   :
	@project  :
	@description :
***************************************** */

/* ------------------------ INCLUDES ---------------------------------------*/

#include "src/_semihosting_prerequirements_check.h"

#include "ARM_api.h"
#include "PRINTF_api.h"
#ifdef CONFIG_ARM_SEMIHOSTING_CONFIG_ENABLE_RX
    #include "os_wrapper.h"
#endif

/* ------------------------ defines ------------------------------*/
#define INSTANCE(hndl)	((SEMIHOSTING_Instance_t*)hndl)

/* ------------------------typedefs ------------------------------*/

/* ---------------------------- External variables ---------------------------------*/

/* ------------------------ External functions ------------------------------*/

/* ------------------------------ Exported variables ---------------------------------*/

/* ------------------------------ Local variables ---------------------------------*/


/* ------------------------ Local Functions  -------------------------------*/


static int BKPT(int op, void* p1, void* p2) ;
extern int smihosting_is_active ;
static int terminal_hndl;
static SEMIHOSTING_Instance_t  sh_instance;


#define SYS_OPEN    	0x01
#define SYS_CLOSE    	0x02
#define SYS_READ    	0x06
#define SYS_READC   	0x07
#define SYS_WRITE		0x05
#define SYS_WRITE0 		0x04
#define SYS_FLEN 		0x0C
#define SYS_TMPNAM 		0x0D
#define SYS_REMOVE 		0x0E



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
int ARM_API_SH_Open(const char* fileName, int mode)
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
 * function ARM_SH_Remove()
 */
int ARM_SH_Remove(char* fileName)
{
  int retVal;
  void* block[2];

  block[0] = (void*)fileName;
  block[1] = (void*)strlen(fileName);
  retVal = BKPT(SYS_REMOVE, (void*) block, (void*) 0);

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
	pdev_descriptor_const   callback_dev;

	callback_dev = INSTANCE(aHandle)->callback_dev;
	ARM_API_SH_Write(terminal_hndl,apData,aLength);
	if(NULL != callback_dev)
	{
		DEV_CALLBACK_1_PARAMS(callback_dev , CALLBACK_TX_DONE,(void*)aLength); // !!! to avoid recursivity in semihosting transmited length should be '>=aLength'
	}
	return aLength;
}

#ifdef CONFIG_ARM_SEMIHOSTING_CONFIG_ENABLE_RX

static uint8_t sh_rx_buffer[CONFIG_ARM_SEMIHOSTING_RX_BUFFER];
const char *windows_sync_file="C:/Temp/1234.txt";
const char *linux_sync_file="/tmp/1234";
const char *p_sync_file_str;

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        create_sync_file                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
static void create_sync_file( SEMIHOSTING_Instance_t *aHandle , int sync_file_hanfler )
{
	PRINT_STR_REPLY(INSTANCE(aHandle)->this_dev , "\r\n\r\nif you want to enter commands over semihosting\r\ndelete ");
	PRINT_STR_REPLY(INSTANCE(aHandle)->this_dev ,p_sync_file_str);
	PRINT_STR_REPLY(INSTANCE(aHandle)->this_dev , " and wait for shell sign\r\n");
	ARM_API_SH_Write(sync_file_hanfler,(const uint8_t*) "a", 1);
	ARM_API_SH_Close(sync_file_hanfler);
}

#define RUN_STATE_PREINIT				0
#define RUN_STATE_FAILED_CREATING_FILE	1
#define RUN_STATE_RUNNING				2

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        test_for_input_ready                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
static void test_for_input_ready( SEMIHOSTING_Instance_t *aHandle )
{
	uint8_t cRead;
	int read_sync_hndl;
	size_t i;
	pdev_descriptor_const   callback_dev;
	static uint8_t run_state = RUN_STATE_PREINIT;


	if (RUN_STATE_PREINIT == run_state)
	{
		p_sync_file_str = windows_sync_file;
		PRINTF_REPLY(INSTANCE(aHandle)->this_dev,"trying to create %s \r\n",p_sync_file_str);
		read_sync_hndl=ARM_API_SH_Open(p_sync_file_str,4);
		if(-1 == read_sync_hndl)
		{
			p_sync_file_str = linux_sync_file;
			PRINTF_REPLY(INSTANCE(aHandle)->this_dev,"trying to create %s \r\n",p_sync_file_str);
			read_sync_hndl=ARM_API_SH_Open(p_sync_file_str,4);
		}


		if (-1 != read_sync_hndl)
		{
			create_sync_file(aHandle,read_sync_hndl);
			run_state = RUN_STATE_RUNNING;
		}
		else
		{
			PRINT_STR_REPLY(INSTANCE(aHandle)->this_dev,"cannot create sync file \r\n");
			run_state = RUN_STATE_FAILED_CREATING_FILE;
		}

		return ;
	}

	if (RUN_STATE_FAILED_CREATING_FILE == run_state)
	{
		return ;
	}


	read_sync_hndl=ARM_API_SH_Open(p_sync_file_str,1);
	if(-1 != read_sync_hndl) // file exists - user dont want to execute any command
	{
		read_sync_hndl=ARM_API_SH_Close(read_sync_hndl);
	}
	else
	{
		PRINTF_REPLY(aHandle->this_dev , "\r\n\r\nenter command (length should be less than %d) \r\n"
				"and press 'enter' till response \r\n>",CONFIG_ARM_SEMIHOSTING_RX_BUFFER);
		ARM_API_SH_Read(terminal_hndl,sh_rx_buffer,CONFIG_ARM_SEMIHOSTING_RX_BUFFER);
		cRead = sh_rx_buffer[0];
		i=1;
		while (('\n' != cRead) && ('\r' != cRead) && (i<CONFIG_ARM_SEMIHOSTING_RX_BUFFER))
		{
			cRead = sh_rx_buffer[i++];
		}
		if(i == (CONFIG_ARM_SEMIHOSTING_RX_BUFFER + 1))
		{
			PRINTF_REPLY(aHandle->this_dev , "error : command should be less then %d chars \r\n",CONFIG_ARM_SEMIHOSTING_RX_BUFFER);
			sh_rx_buffer[0]='\n';
		}

		callback_dev = INSTANCE(aHandle)->callback_dev;
		if (callback_dev )
		{

			DEV_CALLBACK_2_PARAMS(callback_dev , CALLBACK_DATA_RECEIVED,  sh_rx_buffer, (void*)i);
		}

		read_sync_hndl=ARM_API_SH_Open(p_sync_file_str,4);
		if (-1 != read_sync_hndl)
		{
			create_sync_file(aHandle,read_sync_hndl);
		}
	}

}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        poll_for_semihosting_data_task                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
void poll_for_semihosting_data_task( void *aHandle )
{


//	int read_size ;

	os_delay_ms( 5000 );
	for( ;; )
	{
		os_delay_ms( 5000 );

		test_for_input_ready(aHandle);

		os_stack_test();

	}

}

#endif


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
			terminal_hndl=ARM_API_SH_Open(":tt",5);//mode 5=wb

#ifdef CONFIG_ARM_SEMIHOSTING_CONFIG_ENABLE_RX
			if(NULL != aHandle)
			{
				os_create_task("sw_uart_wrapper_task",poll_for_semihosting_data_task,
						aHandle , ARM_SEMIHOSTING_CONFIG_TASK_STACK_SIZE , ARM_SEMIHOSTING_CONFIG_TASK_PRIORITY);
			}
#endif
			break;
		case IOCTL_SET_CALLBACK_DEV:
		case IOCTL_SET_ISR_CALLBACK_DEV:
			INSTANCE(aHandle)->callback_dev =(pdev_descriptor_t) aIoctl_param1;
			break;
		case IOCTL_ARM_SH_CALL_NO_OS_TASK:
			test_for_input_ready(aHandle);
			break;
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
uint8_t  arm_sh_api_init_dev_descriptor(pdev_descriptor_t aDevDescriptor)
{
	if(NULL == aDevDescriptor) return 1;


	aDevDescriptor->handle = &sh_instance;
	aDevDescriptor->ioctl = arm_sh_ioctl;
	aDevDescriptor->pwrite = arm_sh_pwrite;
	sh_instance.callback_dev = NULL;
	sh_instance.this_dev = aDevDescriptor;

	return 0 ;

}

