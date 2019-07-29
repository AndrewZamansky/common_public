/* ****************************************
 *****************************************
	@file   <semihosting.c>

	@date   :
	@project  :
	@description :
***************************************** */

/* ------------------------ INCLUDES ---------------------------------------*/
#include "_project_typedefs.h"
#include "_project_defines.h"

#include "semihosting_api.h"
#include "dev_management_api.h"

//#include "PRINTF_api.h"
#ifdef CONFIG_ARM_SEMIHOSTING_CONFIG_ENABLE_RX
    #include "os_wrapper.h"
#endif

#include "semihosting.h"
#include "stdio.h"
#include "string.h"


/*following line add module to available module list for dynamic device tree*/
#include "semihosting_add_component.h"

/* ------------------------ defines ------------------------------*/

/* ------------------------typedefs ------------------------------*/

/* ---------------------------- External variables -------------------------*/

/* ------------------------ External functions ------------------------------*/

/* ------------------------------ Exported variables -----------------------*/

/* ------------------------------ Local variables ---------------------------*/


/* ------------------------ Local Functions  -------------------------------*/


static int BKPT(int op, void* p1, void* p2) ;
extern int smihosting_is_active ;
static int terminal_hndl = -1;


#define SYS_OPEN      0x01
#define SYS_CLOSE     0x02
#define SYS_READ      0x06
#define SYS_READC     0x07
#define SYS_WRITE     0x05
#define SYS_WRITE0    0x04
#define SYS_FLEN      0x0C
#define SYS_TMPNAM    0x0D
#define SYS_REMOVE    0x0E



/********  types  *********************/

/********  externals *********************/
extern uint32_t bkpt_asm(int op, void* p1, void* p2);


/********  local defs *********************/




void ARM_API_SH_Write0(const uint8_t* ptr)
{
	BKPT(SYS_WRITE0, (void*)ptr, (void*)0);
}


struct smh_open_t
{
	const char *fname;
	size_t mode;
	size_t len;
};
/*
 * function ARM_API_SH_Open()
 */
int ARM_API_SH_Open(const char* fileName, size_t mode)
{
	int retVal;
	struct smh_open_t open_params;

	open_params.fname = fileName;
	open_params.mode = mode;
	open_params.len = strlen(fileName);
	retVal = BKPT(SYS_OPEN, &open_params, (void*)(size_t)0);
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
	retVal = BKPT(SYS_REMOVE, (void*)block, (void*)0);

  return retVal;
}



/*
 * function ARM_API_SH_Close()
 */
int ARM_API_SH_Close(int FileHandle)
{
	return BKPT(SYS_CLOSE, (void*)(size_t)FileHandle, (void*)0);
}


struct smh_read_t
{
	size_t fd;
	void *memp;
	size_t len;
};
/*
 * function ARM_API_SH_Read()
 */
int ARM_API_SH_Read(int FileHandle, uint8_t* pBuffer, size_t NumBytesToRead)
{
	int num_of_bytes_actually_read;
	struct smh_read_t read_params;

	read_params.fd = FileHandle;
	read_params.memp = pBuffer;
	read_params.len = NumBytesToRead;
	num_of_bytes_actually_read = BKPT(SYS_READ, &read_params, (void*)0);

	return num_of_bytes_actually_read;
}


/*
 * function ARM_API_SH_GetFileLength()
 */
int ARM_API_SH_GetFileLength(int FileHandle)
{
	return BKPT(SYS_FLEN, (void*)(size_t)FileHandle, (void*)0);
}


char _SH_ReadC(void)
{
	char c;

	c = BKPT(SYS_READC, (void*)0, (void*)0);
	return c;
}

struct smh_write_t
{
	size_t fd;
	void *memp;
	size_t len;
};

int ARM_API_SH_Write(int FileHandle,
		const uint8_t* pBuffer, size_t NumBytesToWrite)
{
	int num_of_bytes_actually_written;
	struct smh_write_t read_params;

	read_params.fd = FileHandle;
	read_params.memp = pBuffer;
	read_params.len = NumBytesToWrite;
	num_of_bytes_actually_written = BKPT(SYS_WRITE, &read_params, (void*)0);

	return num_of_bytes_actually_written;
}



static int BKPT(int op, void* p1, void* p2)
{
	if(0 == smihosting_is_active)
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
		pBuffer[i] = curr_char;
		i++;
	}

}

/**
 * semihosting_pwrite()
 *
 * return:
 */
size_t semihosting_pwrite( struct dev_desc_t *adev,
		const uint8_t *apData, size_t aLength, size_t aOffset)
{
	struct semihosting_instance_t *handle;
	struct dev_desc_t *   callback_dev;

	if (-1 == terminal_hndl)
	{
		return 0;
	}
	handle = DEV_GET_CONFIG_DATA_POINTER(adev);
	callback_dev = handle->callback_dev;
	ARM_API_SH_Write(terminal_hndl, apData, aLength);
	if(NULL != callback_dev)
	{
		/* !!! to avoid recursivity in
		 * semihosting transmited length should be '>=aLength' */
		DEV_CALLBACK_1_PARAMS(callback_dev, CALLBACK_TX_DONE, (void*)aLength);
	}
	return aLength;
}

#ifdef CONFIG_ARM_SEMIHOSTING_CONFIG_ENABLE_RX

static uint8_t sh_rx_buffer[CONFIG_ARM_SEMIHOSTING_RX_BUFFER];
static const char *windows_sync_file = "C:/Temp/1234.txt";
static const char *linux_sync_file = "/tmp/1234";
static const char *p_sync_file_str;

static void sh_write_string(char *str)
{
	ARM_API_SH_Write(terminal_hndl, str, strlen(str));
}


/**
 * create_sync_file()
 *
 * return:
 */
static void create_sync_file(int sync_file_hanfler )
{
	sh_write_string(
		"\r\n\r\nif you want to enter commands over semihosting\r\ndelete ");
	sh_write_string(p_sync_file_str);
	sh_write_string(" and wait for shell sign\r\n");
	sh_write_string(
			" -- if you cannot delete the file , re-open debugger server \r\n");
	sh_write_string(sync_file_hanfler, (const uint8_t*)"a", 1);
	ARM_API_SH_Close(sync_file_hanfler);
}


#define RUN_STATE_PREINIT               0
#define RUN_STATE_FAILED_CREATING_FILE  1
#define RUN_STATE_RUNNING               2

#define  MAX_STR_LEN   64
static  char  print_str[MAX_STR_LEN];


/**
 * test_for_input_ready()
 *
 * return:
 */
static void test_for_input_ready( struct dev_desc_t *adev  )
{
	int read_sync_hndl;
	struct semihosting_instance_t *handle;

	static uint8_t run_state = RUN_STATE_PREINIT;

	handle = DEV_GET_CONFIG_DATA_POINTER(adev);

	if (RUN_STATE_PREINIT == run_state)
	{
		p_sync_file_str = windows_sync_file;
		snprintf(print_str,
				MAX_STR_LEN, "trying to create %s \r\n", p_sync_file_str);
		sh_write_string(print_str);
		read_sync_hndl=ARM_API_SH_Open(p_sync_file_str, 4);
		if(-1 == read_sync_hndl)
		{
			p_sync_file_str = linux_sync_file;
			snprintf(print_str,
					MAX_STR_LEN, "trying to create %s \r\n", p_sync_file_str);
			sh_write_string(print_str);
			read_sync_hndl=ARM_API_SH_Open(p_sync_file_str, 4);
		}


		if (-1 != read_sync_hndl)
		{
			create_sync_file(adev, read_sync_hndl);
			run_state = RUN_STATE_RUNNING;
		}
		else
		{
			sh_write_string(adev, "cannot create sync file \r\n");
			run_state = RUN_STATE_FAILED_CREATING_FILE;
		}

		return ;
	}

	if (RUN_STATE_FAILED_CREATING_FILE == run_state)
	{
		return ;
	}


	read_sync_hndl=ARM_API_SH_Open(p_sync_file_str, 1);
	// file exists - user dont want to execute any command :
	if(-1 != read_sync_hndl)
	{
		read_sync_hndl=ARM_API_SH_Close(read_sync_hndl);
	}
	else
	{
		uint8_t cRead;
		struct dev_desc_t *   callback_dev;
		size_t i;

		snprintf(print_str, MAX_STR_LEN,
				"\r\n\r\nenter command (length should be less than %d) \r\n"
				"and press 'enter' till response \r\n>",
				CONFIG_ARM_SEMIHOSTING_RX_BUFFER);
		sh_write_string(print_str);
		ARM_API_SH_Read(terminal_hndl,
				sh_rx_buffer,CONFIG_ARM_SEMIHOSTING_RX_BUFFER);
		cRead = sh_rx_buffer[0];
		i = 1;
		while (('\n' != cRead)
				&& ('\r' != cRead) && (i < CONFIG_ARM_SEMIHOSTING_RX_BUFFER))
		{
			cRead = sh_rx_buffer[i++];
		}
		if(i == (CONFIG_ARM_SEMIHOSTING_RX_BUFFER + 1))
		{
			snprintf(print_str, MAX_STR_LEN,
					"error : command should be less then %d chars \r\n",
					CONFIG_ARM_SEMIHOSTING_RX_BUFFER);
			sh_write_string(print_str);
			sh_rx_buffer[0] = '\n';
		}

		callback_dev = handle->callback_dev;
		if (callback_dev )
		{

			DEV_CALLBACK_2_PARAMS(callback_dev,
					CALLBACK_DATA_RECEIVED,  sh_rx_buffer, (void*)i);
		}

		read_sync_hndl = ARM_API_SH_Open(p_sync_file_str, 4);
		if (-1 != read_sync_hndl)
		{
			create_sync_file(adev, read_sync_hndl);
		}
	}

}


/**
 * poll_for_semihosting_data_task()
 *
 * return:
 */
void poll_for_semihosting_data_task( void *aHandle )
{
	os_delay_ms( 5000 );
	for( ;; )
	{
		os_delay_ms( 5000 );

		test_for_input_ready((struct dev_desc_t *)aHandle);

		os_stack_test();
	}
}

#endif


/**
 * semihosting_ioctl()
 *
 * return:
 */
uint8_t semihosting_ioctl( struct dev_desc_t *adev,
		const uint8_t aIoctl_num, void * aIoctl_param1, void * aIoctl_param2)
{
	struct semihosting_instance_t *handle;

	handle = DEV_GET_CONFIG_DATA_POINTER(adev);
	switch(aIoctl_num)
	{
		case IOCTL_DEVICE_START :
			// some debuggers expect mode = 'wb'(5) and some expect 'w'(4)
			// so checking two modes
			terminal_hndl = ARM_API_SH_Open(":tt", 5);//mode 5=wb
			if (-1 == terminal_hndl)
			{
				terminal_hndl = ARM_API_SH_Open(":tt", 4);//mode 4=w
			}
#ifdef CONFIG_ARM_SEMIHOSTING_CONFIG_ENABLE_RX
			os_create_task("sw_uart_wrapper_task",
					poll_for_semihosting_data_task, adev,
					ARM_SEMIHOSTING_CONFIG_TASK_STACK_SIZE,
					ARM_SEMIHOSTING_CONFIG_TASK_PRIORITY);
#endif
			break;
		case IOCTL_SET_CALLBACK_DEV:
		case IOCTL_SET_ISR_CALLBACK_DEV:
			handle->callback_dev = (struct dev_desc_t *)aIoctl_param1;
			break;
#ifdef CONFIG_ARM_SEMIHOSTING_CONFIG_ENABLE_RX
		case IOCTL_ARM_SH_CALL_NO_OS_TASK:
			test_for_input_ready(adev);
			break;
#endif
		default :
			return 1;
	}
	return 0;
}
