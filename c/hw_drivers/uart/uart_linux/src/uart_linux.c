/*
 *
 * uart_linux.c

 *
 */

/********  includes *********************/

#include "_project_typedefs.h"
#include "_project_defines.h"

#include "dev_management_api.h"

#include "uart_linux.h"

#include <termios.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#include "uart_api.h"

#include <pthread.h>

/*following line add module to available module list for dynamic device tree*/
#include "uart_linux_add_component.h"

/********  defines *********************/
#define READ_BUFF_LEN  255

#define MAX_ERR_STR_LEN   255
#define ALREADY_INITIALIZED  0x34

//#define DEBUG_UART
//#define PRINT_BIN_DATA

/********  types  *********************/


/* ------------ External variables ---------------------------------*/

/* ------------------------ External functions ------------*/

/* ------------------------ Exported variables --------*/

char error_str[MAX_ERR_STR_LEN + 1];

#ifdef DEBUG_UART
static char uart_send_dbg_str[] = "uart_linux send start:\n";
uint8_t _do_uart_dbg_print = 1;
static void dbg_print(uint8_t const *data, size_t len)
{
	if (0 == _do_uart_dbg_print) return;

	#ifdef PRINT_BIN_DATA
		size_t i;
		for (i = 0; i < len; i++)
		{
			char dbg_str[] = "0x00 ";
			snprintf(dbg_str, sizeof(dbg_str), "0x%02x ", *data);
			write(1, dbg_str, sizeof(dbg_str) - 1);
			data++;
		}
	#else
		write(1, data, len);
	#endif
}
#endif


/**
 * uart_linux_pwrite()
 *
 * return:
 */
size_t uart_linux_pwrite(struct dev_desc_t *adev,
			const uint8_t *apData, size_t aLength, size_t aOffset)
{
	struct uart_linux_cfg_t *cfg_hndl;
	struct uart_linux_runtime_t *runtime_handle;
	int tty_fd;
	struct dev_desc_t * callback_tx_dev ;
	size_t written_size;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(adev);
	runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(adev);

	tty_fd = runtime_handle->tty_fd;
	callback_tx_dev = cfg_hndl->callback_tx_dev;
#ifdef DEBUG_UART
	if (0 != _do_uart_dbg_print)
	{
		write(1, uart_send_dbg_str, sizeof(uart_send_dbg_str) - 1);
	}
	dbg_print(apData, aLength>16 ? 32 : aLength);//TODO : remove condition
#endif

	written_size = write (tty_fd, apData, aLength);

#ifdef DEBUG_UART
	if (0 != _do_uart_dbg_print)
	{
		char uart_send_end_dbg_str[] = "uart_linux send written_size = 00000\n";
		snprintf(uart_send_end_dbg_str, sizeof(uart_send_end_dbg_str),
				"uart_linux send written_size = %05lu\n", written_size);
		write(1, uart_send_end_dbg_str, sizeof(uart_send_end_dbg_str) - 1);
	}
#endif

	if (NULL != callback_tx_dev)
	{
		DEV_CALLBACK_1_PARAMS(
				callback_tx_dev, CALLBACK_TX_DONE, (void*)written_size);
	}

}


static void *receive_thread(void *adev)
{
	struct uart_linux_cfg_t *cfg_hndl;
	struct uart_linux_runtime_t *runtime_handle;
	int curr_read_num;
	char rd_buf[READ_BUFF_LEN + 1];
	int tty_fd;
	struct dev_desc_t * callback_rx_dev ;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(adev);
	runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(adev);

	callback_rx_dev = cfg_hndl->callback_rx_dev;
	tty_fd = runtime_handle->tty_fd;
	while (1)
	{
		curr_read_num = read (tty_fd, rd_buf, READ_BUFF_LEN);
		if (0 > curr_read_num)
		{
			CRITICAL_ERROR("linux uart read error");
			break;
		}
#ifdef DEBUG_UART
		if (0 == curr_read_num)
		{
			write(1, "-X0X-", sizeof("-X0X-" - 1));
		}
		else
		{
			dbg_print(rd_buf, curr_read_num);
		}
		if (0 != _do_uart_dbg_print)
		{
			write(1, "|e.rcv|\n", sizeof("|e.rcv|\n" - 1));
		}
#endif
		if (NULL != callback_rx_dev)
		{
			DEV_CALLBACK_2_PARAMS( callback_rx_dev,
				CALLBACK_DATA_RECEIVED, rd_buf, (void*)(size_t)curr_read_num);
		}

	}
}


static void set_baud_rate(
		struct uart_linux_runtime_t *runtime_handle, uint32_t baud_rate)
{
	struct termios tty;
	speed_t baud_rate_constant;
	int tty_fd;

	tty_fd = runtime_handle->tty_fd;
	if (tcgetattr (tty_fd, &tty) != 0)
	{
		snprintf(error_str, MAX_ERR_STR_LEN, "error %d from tcgetattr", errno);
		CRITICAL_ERROR(error_str);
	}
	switch (baud_rate)
	{
	case 9600:
		baud_rate_constant = B9600;
		break;
	case 115200:
		baud_rate_constant = B115200;
		break;
	case 921600:
		baud_rate_constant = B921600;
		break;
	default :
		CRITICAL_ERROR("not supported baud rate");
		return;
	}
	cfsetospeed (&tty, baud_rate_constant);
	cfsetispeed (&tty, baud_rate_constant);

	if (tcsetattr (tty_fd, TCSANOW, &tty) != 0)
	{
		snprintf(error_str, MAX_ERR_STR_LEN, "error %d from tcsetattr", errno);
		CRITICAL_ERROR(error_str);
	}

}


static void uart_start(struct dev_desc_t *adev,
		struct uart_linux_cfg_t *cfg_hndl,
		struct uart_linux_runtime_t *runtime_handle)
{
	struct termios tty;
	char *portname;
	int tty_fd;
	int parity;
	int err;

	if (ALREADY_INITIALIZED == runtime_handle->initialized)
	{
		return;
	}

	runtime_handle->initialized = ALREADY_INITIALIZED;

	error_str[MAX_ERR_STR_LEN] = 0;

	memset (&tty, 0, sizeof tty);

	parity = 0;
	portname = cfg_hndl->tty_dev_path;

	tty_fd = open (portname, O_RDWR | O_NOCTTY | O_SYNC);

	runtime_handle->tty_fd = tty_fd;
	if (tty_fd < 0)
	{
		snprintf(error_str, MAX_ERR_STR_LEN,
				"error %d opening %s: %s", errno, portname, strerror(errno));
		CRITICAL_ERROR(error_str);
		return;
	}


	set_baud_rate(runtime_handle, cfg_hndl->baud_rate);

	if (tcgetattr (tty_fd, &tty) != 0)
	{
		snprintf(error_str, MAX_ERR_STR_LEN, "error %d from tcgetattr", errno);
		CRITICAL_ERROR(error_str);
	}

	tty.c_lflag = 0;                // no signaling chars, no echo,
									// no canonical processing


	// disable IGNBRK for mismatched speed tests; otherwise receive break
	// as \000 chars

	tty.c_cflag &= ~CSTOPB;
	tty.c_cflag &= ~(PARENB);      // shut off parity

	tty.c_cflag &= ~CSIZE;
	tty.c_cflag |=  CS8;     // 8-bit chars
	tty.c_cflag &= ~(PARENB);      // shut off parity

	tty.c_cflag &= ~CRTSCTS;
//	tty.c_cflag |= CRTSCTS;
	tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,

	tty.c_iflag &= ~INLCR; // disable translate NL to CR on input
	tty.c_iflag &= ~ICRNL; // disable translate carriage return to newline
	tty.c_iflag &= ~IGNBRK;  // disable break processing
	tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

	//tty.c_iflag |= IGNPAR;
	tty.c_iflag &= ~IGNPAR;
	tty.c_iflag |= IGNBRK;

	tty.c_oflag = 0;                // no remapping, no delays

	/* fetch bytes as they become available */
	tty.c_cc[VMIN] = 1;
	tty.c_cc[VTIME] = 1;

	tcflush(tty_fd, TCIOFLUSH);

	if (tcsetattr (tty_fd, TCSANOW, &tty) != 0)
	{
		snprintf(error_str, MAX_ERR_STR_LEN, "error %d from tcsetattr", errno);
		CRITICAL_ERROR(error_str);
	}

	err = pthread_create(
			&runtime_handle->thread_id, NULL, &receive_thread, adev);
	if (err != 0)
		printf("\ncan't create thread :[%s]\n", strerror(err));

}


/**
 * uart_linux_ioctl()
 *
 * return:
 */
uint8_t uart_linux_ioctl(struct dev_desc_t *adev, uint8_t aIoctl_num,
		void * aIoctl_param1, void * aIoctl_param2)
{
	struct uart_linux_cfg_t *cfg_hndl;
	struct uart_linux_runtime_t *runtime_handle;
	struct termios tty;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(adev);
	runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(adev);

	switch(aIoctl_num)
	{
	case IOCTL_UART_SET_BAUD_RATE :
		cfg_hndl->baud_rate = *(uint32_t*)aIoctl_param1;
		set_baud_rate(runtime_handle, cfg_hndl->baud_rate);
		break;
	case IOCTL_DEVICE_START :
		uart_start(adev, cfg_hndl, runtime_handle);
		break;

	case IOCTL_UART_DISABLE_TX :
		break;

	case IOCTL_UART_ENABLE_TX :
		break;

	case IOCTL_UART_SET_ISR_CALLBACK_TX_DEV:
		cfg_hndl->callback_tx_dev =(struct dev_desc_t *) aIoctl_param1;
		break;

	case IOCTL_UART_SET_ISR_CALLBACK_RX_DEV:
		cfg_hndl->callback_rx_dev =(struct dev_desc_t *) aIoctl_param1;
		break;

	default :
		return 1;
	}
	return 0;
}
