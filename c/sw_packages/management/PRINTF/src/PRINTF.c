/*
 *
 * file :   PRINTF.c
 *
 *
 */



/********  includes *********************/
#include "project_config.h"
#include "_project_typedefs.h"
#include "_project_defines.h"

#include "dev_management_api.h"

#include "PRINTF.h"
#include "PRINTF_api.h"

#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#if defined(COMPILING_FOR_LINUX_HOST)
	#include <unistd.h>
#endif
/********  defines *********************/
#define MAX_NUM_OF_PRINTF_INSTANCES 4
/********  types  *********************/


/********  externals *********************/


/********  local defs *********************/

#if (CONFIG_USE_MINIMAL_PRINTF)

struct dev_desc_t const *print_dev = NULL;

#else

#include "os_wrapper.h"

#define  SIZE_OF_DEBUG_BUFFER  (1 << (2 + CONFIG_DBG_BUF_SHIFT))
#define  MODULO_BY_BUFFER_SIZE(a)  ((a) & (SIZE_OF_DEBUG_BUFFER - 1))

os_mutex_t  print_dbg_mutex;

static struct dev_desc_t *debug_out_devs[MAX_NUM_OF_PRINTF_INSTANCES] = {NULL};
static struct dev_desc_t *note_out_devs[MAX_NUM_OF_PRINTF_INSTANCES] = {NULL};
static uint8_t mutex_init_done = 0;

// keep this buffer global for easier search during debugging
uint8_t  __debug_buffer[SIZE_OF_DEBUG_BUFFER];

static size_t  write_pointer = 1;
static size_t  read_pointer = 0;
static uint8_t  busy_for_isr = 0;

static uint8_t add_dev(
		struct dev_desc_t  **out_devs, struct dev_desc_t *aDevHandle)
{
	uint8_t i;

	for (i = 0; i < MAX_NUM_OF_PRINTF_INSTANCES; i++)
	{
		if ( (NULL == out_devs[i]) || (aDevHandle == out_devs[i]) )
		{
			out_devs[i] = aDevHandle;
			return 0;
		}
	}
	return 1;
}


/**
 * PRINTF_API_AddDebugOutput()
 *
 * return:
 */
uint8_t PRINTF_API_AddDebugOutput(struct dev_desc_t *aDevHandle)
{
	return add_dev(debug_out_devs, aDevHandle);
}


/**
 * PRINTF_API_AddNoteOutput()
 *
 * return:
 */
uint8_t PRINTF_API_AddNoteOutput(struct dev_desc_t *aDevHandle )
{
	return add_dev(note_out_devs, aDevHandle);
}




static uint8_t remove_dev(
		struct dev_desc_t  **out_devs, struct dev_desc_t *aDevHandle)
{
	uint8_t i;

	for (i = 0; i < MAX_NUM_OF_PRINTF_INSTANCES; i++)
	{
		if(aDevHandle == out_devs[i])
		{
			out_devs[i] = NULL;
		}
	}
	return 0;
}


/**
 * PRINTF_API_RemoveDebugOutput()
 *
 * return:
 */
uint8_t PRINTF_API_RemoveDebugOutput(struct dev_desc_t *aDevHandle)
{
	return remove_dev(debug_out_devs, aDevHandle);
}


/**
 * PRINTF_API_RemoveNoteOutput()
 *
 * return:
 */
uint8_t PRINTF_API_RemoveNoteOutput(struct dev_desc_t *aDevHandle )
{
	return remove_dev(note_out_devs, aDevHandle);
}


/**
 * PRINTF_API_init()
 *
 * return:
 */
void PRINTF_API_init()
{
	if (0 == mutex_init_done)
	{
		mutex_init_done = 1;
		print_dbg_mutex = os_create_mutex();
	}
}


/**
 * sendDebugOrNote()
 *
 * return:
 */
static void sendDebugOrNote(struct dev_desc_t  **out_devs,
		const uint8_t* pBuffer,uint32_t aLen)
{
	uint8_t i;

	for (i = 0; i < MAX_NUM_OF_PRINTF_INSTANCES; i++)
	{
		if (NULL != out_devs[i])
		{
			DEV_WRITE(out_devs[i], pBuffer, aLen);
		}
	}
}


size_t PRINTF_API_print_from_debug_buffer(size_t num_of_bytes_to_print)
{
	size_t  curr_write_pointer;// cannot use read_pointer directly (may changes)
	size_t  start_read_pointer;
	size_t  send_len;
	size_t  left_data_size;

	curr_write_pointer = write_pointer;
	start_read_pointer = MODULO_BY_BUFFER_SIZE(read_pointer + 1);
	if ( (0 == num_of_bytes_to_print) ||
			(start_read_pointer == curr_write_pointer))
	{
		return 0;// no data to send
	}

	if (start_read_pointer < curr_write_pointer)
	{
		send_len = curr_write_pointer - start_read_pointer;
		left_data_size = send_len;
	}
	else
	{
		send_len = SIZE_OF_DEBUG_BUFFER - start_read_pointer;
		left_data_size = curr_write_pointer + send_len;
	}

	if (num_of_bytes_to_print < send_len)
	{
		send_len = num_of_bytes_to_print;
	}
	sendDebugOrNote(
			debug_out_devs, &__debug_buffer[start_read_pointer], send_len);
#if defined(COMPILING_FOR_HOST)
	write(1, &__debug_buffer[start_read_pointer], send_len);
#endif
	start_read_pointer = MODULO_BY_BUFFER_SIZE(start_read_pointer + send_len);
	num_of_bytes_to_print -= send_len;
	left_data_size -= send_len;
	if (0 == start_read_pointer)
	{// read_pointer was wrapped
		if ((0 == curr_write_pointer) || (0 == num_of_bytes_to_print))
		{//no data to send
			read_pointer = SIZE_OF_DEBUG_BUFFER - 1;
		}
		else
		{// there is more data do send
			send_len = curr_write_pointer;
			if (num_of_bytes_to_print < send_len)
			{
				send_len = num_of_bytes_to_print;
			}
			sendDebugOrNote(debug_out_devs, __debug_buffer, send_len);
		#if defined(COMPILING_FOR_HOST)
			write(1, __debug_buffer, send_len);
		#endif
			left_data_size -= send_len;
			read_pointer = (send_len - 1);
		}
	}
	else
	{
		read_pointer = start_read_pointer - 1;
	}
	return  left_data_size;
}


static void update_dbg_buffer(const uint8_t* pBuffer, uint32_t aLen)
{
	size_t copy_len;
	size_t curr_read_pointer;// cannot use read_pointer directly (may changes)

	curr_read_pointer = read_pointer;
	if (curr_read_pointer == write_pointer)// buffer is full
	{
		return;
	}

	if (curr_read_pointer > write_pointer)
	{
		copy_len = curr_read_pointer - write_pointer;
	}
	else// cannot be  (read_pointer == write_pointer),  already checked before
	{
		copy_len = SIZE_OF_DEBUG_BUFFER - write_pointer;
	}
	if (aLen < copy_len)
	{
		copy_len = aLen;
	}
	memcpy(&__debug_buffer[write_pointer], pBuffer, copy_len);
	aLen -= copy_len;
	write_pointer = MODULO_BY_BUFFER_SIZE(write_pointer + copy_len);
	pBuffer += copy_len;

	if ((0 == write_pointer) && (0 != curr_read_pointer))
	{// write_pointer was wrapped and there is still place for new data
		copy_len = curr_read_pointer;
		if (aLen < copy_len)
		{
			copy_len = aLen;
		}
		memcpy(__debug_buffer, pBuffer, copy_len);
		aLen -= copy_len;
		write_pointer += copy_len;
	}
}


/**
 * PRINTF_print_data()
 *
 * return:
 */
void PRINTF_print_data(
		enum PRINTF_TYPE_e aPrntType, const uint8_t* data, uint32_t aLen)
{
	if (0 == aLen) return ;
	switch(aPrntType)
	{
		case PRINTF_TYPE_DBG :
			if (0 == mutex_init_done)
			{
				return;
			}
			if (OS_MUTEX_TAKE_SUCCESS !=
					os_mutex_take_infinite_wait(print_dbg_mutex))
			{
				return;
			}
			busy_for_isr = 1;// protection only for isr(or nested isr)
			update_dbg_buffer(data, aLen);
			busy_for_isr = 0;
			os_mutex_give(print_dbg_mutex);
			break;
		case PRINTF_TYPE_DBG_ISR :
			if (0 == busy_for_isr)
			{
				busy_for_isr = 1;// protection only for isr(or nested isr)
				update_dbg_buffer(data, aLen);
				busy_for_isr = 0;
			}
			break;
		case PRINTF_TYPE_NOTE :
			sendDebugOrNote(note_out_devs, data, aLen);
			break;
		default:
			break;
	}
}


/**
 * PRINTF_print_str()
 *
 * return:
 */
void PRINTF_print_str(enum PRINTF_TYPE_e aPrntType, const char* str)
{
	PRINTF_print_data(aPrntType, (uint8_t*)str, strlen(str));
}


#ifndef _DONT_USE_PRINTF

#define PRINTF_BUF_LENGTH  64
/**
 * PRINTF_printf()
 *
 * return:
 */
void PRINTF_printf(enum PRINTF_TYPE_e aPrntType, const char* Format, ...)
{
	va_list args;
	int retVal;
	char buffer[PRINTF_BUF_LENGTH];

	va_start(args, Format);
	retVal = vsnprintf(
			(char*)buffer, PRINTF_BUF_LENGTH, Format, args);
	va_end(args);

	if (0 >= retVal) return ;

	PRINTF_print_str(aPrntType, buffer);
}


#endif


#endif
