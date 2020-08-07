/******************************************************************************
 * @file     file_descriptor_manager.c
 * @brief
 *
 * @note
 *
 ******************************************************************************/

/*!<Includes */
#include "_project_typedefs.h"
#include "_project_defines.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "errno.h"

#include "dev_management_api.h"
#include "file_descriptor_manager_api.h"
#ifdef CONFIG_USE_INTERNAL_SOCKETS_IMPLEMENTATION
	#include "std_net_functions_api.h"
#endif

#include "os_wrapper.h"
#include "winsock2.h"

//#define DEBUG
#include "PRINTF_api.h"

#if (FD_SETSIZE < CONFIG_MAX_NUMBER_OF_OPENED_FILE_DESCRIPTORS)
{
	#error "to use 'select' function number of descriptors should be less then FD_SETSIZE"
}
#endif

static struct file_desc_t
			file_desc_arr[CONFIG_MAX_NUMBER_OF_OPENED_FILE_DESCRIPTORS] = {{0}};

static os_mutex_t fd_mutex = NULL;


uint8_t file_descriptor_manager_api_init()
{
	fd_mutex = os_create_mutex();
	return 0;
}

static struct file_desc_t* get_file_desc(int fd)
{
	if ((CONFIG_MAX_NUMBER_OF_OPENED_FILE_DESCRIPTORS <= fd) ||
			(0 == file_desc_arr[fd].used))

	{
		//PRINTF_DBG("%s :  fd = %d\n",__FUNCTION__, fd);
		CRITICAL_ERROR("bad socket fd");
		return NULL;
	}
	return &file_desc_arr[fd];
}


#ifdef CONFIG_USE_INTERNAL_SOCKETS_IMPLEMENTATION

#define  MAX_NUM_OF_INET_DEVS  1
static struct dev_desc_t *underlying_INET_dev_arr[MAX_NUM_OF_INET_DEVS] = {0};
#define  MAX_NUM_OF_IPC_DEVS  1
static struct dev_desc_t *underlying_IPC_dev_arr[MAX_NUM_OF_IPC_DEVS] = {0};

uint8_t file_descriptor_manager_api_register_INET_device(
											struct dev_desc_t * ap_dev)
{
	int i;
	int ret_val;

	if (NULL == fd_mutex)
	{
		CRITICAL_ERROR("file descriptor manager not initialized");
	}
	os_mutex_take_infinite_wait(fd_mutex);

	for (i = 0; i < MAX_NUM_OF_INET_DEVS; i++)
	{
		if (NULL == underlying_INET_dev_arr[i])
		{
			underlying_INET_dev_arr[i] = ap_dev;
			ret_val = 0;
			goto exit;
		}
	}
	ret_val = -1;// no free descriptor
exit :
	os_mutex_give(fd_mutex);
	return ret_val;
}

uint8_t file_descriptor_manager_api_register_IPC_device(
											struct dev_desc_t * ap_dev)
{
	int i;
	int ret_val;

	if (NULL == fd_mutex)
	{
		CRITICAL_ERROR("file descriptor manager not initialized");
	}
	os_mutex_take_infinite_wait(fd_mutex);

	for (i = 0; i < MAX_NUM_OF_IPC_DEVS; i++)
	{
		if (NULL == underlying_IPC_dev_arr[i])
		{
			underlying_IPC_dev_arr[i] = ap_dev;
			ret_val = 0;
			goto exit;
		}
	}
	ret_val = -1;// no free descriptor
exit :
	os_mutex_give(fd_mutex);
	return ret_val;
}



static int get_free_file_desc(struct file_desc_t  **file_desc)
{
	size_t i;
	for (i = 0; i < CONFIG_MAX_NUMBER_OF_OPENED_FILE_DESCRIPTORS; i++)
	{
		*file_desc = &file_desc_arr[i];
		if (0 == file_desc_arr[i].used)
		{
			file_desc_arr[i].used = 1;
			PRINTF_DBG("%s : open socket fd = %d\n",__FUNCTION__, i);
			return i;
		}
	}
	return -1;
}


int socket(int socket_family, int socket_type, int protocol)
{
	int ret_val;
	struct file_desc_t  *file_desc;

	if (NULL == fd_mutex)
	{
		CRITICAL_ERROR("file descriptor manager not initialized");
	}
	os_mutex_take_infinite_wait(fd_mutex);

	switch (socket_family)
	{
		case AF_INET:
			break;
		case AF_IPC:
			break;
		default:
			PRINTF_DBG("socket_family = %d\n", socket_family);
			CRITICAL_ERROR("this socket family not implemented yet");
			ret_val = -1;
			goto exit;
	}

	if (AF_INET == socket_family)
	{
		switch (socket_type)
		{
			case SOCK_STREAM:
				break;
			default:
				CRITICAL_ERROR("this socket type not implemented yet");
				ret_val = -1;
				goto exit;
		}

		switch (protocol)
		{
			case IPPROTO_IP:
			case IPPROTO_TCP:
				break;
			default:
				CRITICAL_ERROR("this protocol not implemented yet");
				ret_val = -1;
				goto exit;
		}
	}

	ret_val = get_free_file_desc(&file_desc);
	if (-1 != ret_val)
	{
		file_desc->socket_family = socket_family;
		file_desc->socket_type = socket_type;
		file_desc->protocol = protocol;
	}
exit :
	os_mutex_give(fd_mutex);
	return ret_val;
}


int closesocket(int sockfd)
{
	int ret_val;
	struct file_desc_t  *curr_fd;

	os_mutex_take_infinite_wait(fd_mutex);
	curr_fd = get_file_desc(sockfd);
	if (NULL == curr_fd)
	{
		ret_val = -1;
		goto exit;
	}

	ret_val = -1;
	if (NULL != curr_fd->underlying_dev)
	{
		ret_val = curr_fd->file_desc_ops.closesocket_func(
												curr_fd->internal_desc);
	}
	if (0 == ret_val)
	{
		curr_fd->used = 0;
		curr_fd->underlying_dev = NULL;
	}
exit:
	os_mutex_give(fd_mutex);
	return ret_val;
}


int connect(
		int sockfd, const struct sockaddr *addr, unsigned int addrlen)
{
	struct file_descriptor_manager_ioctl_socket_open_t  sock_open;
	struct file_desc_t  *curr_fd;
	int ret_val;
	size_t dev_arr_size;
	struct dev_desc_t **underlying_dev_arr;
	struct dev_desc_t *underlying_dev;
	uint16_t i;

	curr_fd = get_file_desc(sockfd);
	if (NULL == curr_fd)
	{
		return -1;
	}

	switch (curr_fd->socket_family)
	{
		case AF_INET:
			underlying_dev_arr = underlying_INET_dev_arr;
			dev_arr_size = MAX_NUM_OF_INET_DEVS;
			break;
		case AF_IPC:
			underlying_dev_arr = underlying_IPC_dev_arr;
			dev_arr_size = MAX_NUM_OF_IPC_DEVS;
			break;
		default :
			return -1;
	}

	ret_val = -1;
	sock_open.new_socket_descriptor = curr_fd;
	for (i = 0; i < dev_arr_size; i++)
	{
		underlying_dev = underlying_dev_arr[i];
		if (NULL == underlying_dev)
		{
			continue;
		}

		if (0 == DEV_IOCTL_1_PARAMS(underlying_dev,
										IOCTL_OPEN_SOCKET, &sock_open))
		{
			ret_val = curr_fd->file_desc_ops.connect_func(
								curr_fd->internal_desc, addr, addrlen);
			if (0 <= ret_val)
			{
				curr_fd->underlying_dev = underlying_dev;
				return 0; // connection succeed so we can exit
			}
			else
			{
				ret_val = curr_fd->file_desc_ops.closesocket_func(
													curr_fd->internal_desc);
			}
		}
	}
	return -1;
}


ssize_t recv(int sockfd, void *buf, size_t len, int flags)
{
	struct file_desc_t  *curr_fd;
	uint32_t socket_options;
	uint8_t read_ready;
	ssize_t size_received;
	uint8_t  retVal;

	curr_fd = get_file_desc(sockfd);
	if ((NULL == curr_fd) || (NULL == curr_fd->underlying_dev))
	{
		return -1;
	}

	socket_options = curr_fd->socket_options;

	retVal = curr_fd->file_desc_ops.is_data_available_func(
								curr_fd->internal_desc, &read_ready);
	if (0 != retVal)
	{
		errno = ENOTCONN;
		return -1;
	}

	if ((0 == read_ready) &&
			( (flags & MSG_DONTWAIT) ||
			(socket_options & (1 << SO_NONBLOCK)) )   )
	{
		errno = EAGAIN;
		return -1;
	}

	size_received = curr_fd->file_desc_ops.recv_func(
									curr_fd->internal_desc, buf, len, flags);

	if (0 > size_received)
	{
		errno = ENOTCONN;
		return -1;
	}
	return size_received;
}


int getsockopt(int sockfd, int level, int optname,
									void *optval, socklen_t *optlen)
{
	struct file_desc_t  *curr_fd;
	uint32_t socket_options;
	int  int_val;

	curr_fd = get_file_desc(sockfd);
	if (NULL == curr_fd)
	{
		return -1;
	}

	socket_options = curr_fd->socket_options;

	if (SOL_SOCKET != level)
	{
		CRITICAL_ERROR("unknown option level");
	}
	if (sizeof(int) > *optlen)
	{
		CRITICAL_ERROR("bad option size");
	}

	switch(optname)
	{
		case SO_ERROR:
			break;
		case SO_NONBLOCK:
			break;

		default:
			CRITICAL_ERROR("this option not implemented yet");
	}

	int_val = (socket_options & ( 1 << optname)) ? 1 : 0;
	if (sizeof(long) == *optlen)
	{
		*(long*)optval = int_val;
	}
	else
	{
		*(int*)optval = int_val;
	}
	return 0;
}


static uint8_t set_tcp_level_option(int optname, const void *optval,
		socklen_t optlen, uint32_t *socket_options,
		uint8_t *need_to_update_low_level)
{
	*need_to_update_low_level = 0;
	switch(optname)
	{
		case TCP_NODELAY:
			*need_to_update_low_level = 1;
			break;
		default:
			CRITICAL_ERROR("this tcp option is not implemented yet");
	}
	return 0;
}


static int set_socket_level_option(struct file_desc_t  *curr_fd,
						int optname, const void *optval, socklen_t optlen,
						uint8_t *need_to_update_low_level)
{
	uint32_t val;
	uint32_t socket_options;

	*need_to_update_low_level = 0;
	socket_options = curr_fd->socket_options;

	switch(optname)
	{
		case SO_ERROR:
			break;
		case SO_BINDTODEVICE:
		{
			struct dev_desc_t * underlying_dev;
			struct file_descriptor_manager_ioctl_socket_open_t  sock_open;
#if !defined(CONFIG_LINUX_HOST_GCC)
			underlying_dev = DEV_OPEN((char*)optval);
#else
	#warning "change device management to be independent of compiler"
#endif
			if (NULL == underlying_dev) return -1;

			sock_open.new_socket_descriptor = curr_fd;
			if (0 == DEV_IOCTL_1_PARAMS(underlying_dev,
											IOCTL_OPEN_SOCKET, &sock_open))
			{
				curr_fd->underlying_dev = underlying_dev;
			}
			else
			{
				return -1;
			}
			break;
		}
		case SO_NONBLOCK:
			break;
		case SO_KEEPALIVE:
			*need_to_update_low_level = 1;
			// TODO : ESP32 has keep alive parameter for port, ESP8266 doesn't
			break;

		default:
			CRITICAL_ERROR("this socket option is not implemented yet");
	}

	if (sizeof(long) == optlen)
	{
		val = (*(long *)optval) ? 1 : 0 ;
	}
	else
	{
		val = (*(int *)optval) ? 1 : 0 ;
	}

	socket_options &= ~(1 << optname);
	socket_options |= (val << optname);
	curr_fd->socket_options = socket_options;
	return 0;
}


int setsockopt(int sockfd, int level,
		int optname, const void *optval, socklen_t optlen)
{
	struct file_desc_t  *curr_fd;
	uint32_t *socket_options;
	uint8_t need_to_update_low_level;
	int ret_val;

	curr_fd = get_file_desc(sockfd);
	if (NULL == curr_fd)
	{
		return -1;
	}

	ret_val = -1;
	socket_options = &curr_fd->socket_options;
	switch(level)
	{
	case SOL_SOCKET:
		 ret_val = set_socket_level_option(
				curr_fd, optname, optval, optlen, &need_to_update_low_level);
		break;
	case IPPROTO_TCP:
		ret_val = set_tcp_level_option(
			optname, optval, optlen, socket_options, &need_to_update_low_level);
		break;

	default:
		CRITICAL_ERROR("unknown option level");
	}
	if (-1 == ret_val)
	{
		return -1;
	}

	if ((1 == need_to_update_low_level)  && (NULL != curr_fd->underlying_dev))
	{
		return curr_fd->file_desc_ops.setsockopt_func(
				curr_fd->internal_desc, level, optname, optval, optlen);
	}
	return 0;
}


int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
	struct file_desc_t  *curr_fd;

	curr_fd = get_file_desc(sockfd);
	if ((NULL == curr_fd) || (NULL == curr_fd->underlying_dev))
	{
		return -1;
	}

	return curr_fd->file_desc_ops.bind_func(
					curr_fd->internal_desc, addr, addrlen);
}


int getsockname(
		int sockfd, struct sockaddr *local_addr, socklen_t *addrlen)
{
	struct file_desc_t  *curr_fd;

	curr_fd = get_file_desc(sockfd);
	if ((NULL == curr_fd) || (NULL == curr_fd->underlying_dev))
	{
		return -1;
	}

	return curr_fd->file_desc_ops.getpeername_func(
								curr_fd->internal_desc, local_addr, addrlen);
}


int getpeername(
		int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
	struct file_desc_t  *curr_fd;

	curr_fd = get_file_desc(sockfd);
	if ((NULL == curr_fd) || (NULL == curr_fd->underlying_dev))
	{
		return -1;
	}

	return curr_fd->file_desc_ops.getpeername_func(
									curr_fd->internal_desc, addr, addrlen);
}


ssize_t send(
		int sockfd, const void *buffer, size_t length, int flags)
{
	struct file_desc_t  *curr_fd;

	curr_fd = get_file_desc(sockfd);
	if ((NULL == curr_fd) || (NULL == curr_fd->underlying_dev))
	{
		return -1;
	}

	if(0 != flags)
	{
		CRITICAL_ERROR("flags != 0 not implemented yet");
	}

	return curr_fd->file_desc_ops.send_func(curr_fd->internal_desc,
													buffer, length, flags);
}


int listen(int sockfd, int backlog)
{
	struct file_desc_t  *curr_fd;

	curr_fd = get_file_desc(sockfd);
	if ((NULL == curr_fd) || (NULL == curr_fd->underlying_dev))
	{
		return -1;
	}

	return curr_fd->file_desc_ops.listen_func(curr_fd->internal_desc, backlog);
}


int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
	struct file_desc_t  *main_fd;
	struct file_desc_t  *new_fd;
	struct file_desc_t  *new_internal_fd;
	int new_fd_num;

	main_fd = get_file_desc(sockfd);
	if ((NULL == main_fd) || (NULL == main_fd->underlying_dev))
	{
		return -1;
	}

	if (sizeof(int) != sizeof(void*))
	{
		CRITICAL_ERROR("not supported types");
	}
	new_internal_fd =
		(struct file_desc_t*)(size_t)main_fd->file_desc_ops.accept_func(
							main_fd->internal_desc, addr, addrlen);

	if (-1 == (int)((size_t)new_internal_fd))
	{
		return -1;
	}

	new_fd_num = get_free_file_desc(&new_fd);
	if (-1 == new_fd_num)
	{
		main_fd->file_desc_ops.closesocket_func(new_internal_fd);
		return -1;
	}
	memcpy(new_fd, main_fd, sizeof(struct file_desc_t));
	new_fd->internal_desc = new_internal_fd;

	return new_fd_num;
}


int shutdown(int socket, int how)
{
	CRITICAL_ERROR("shutdown not implemented yet");
	return 0;
}


int WSAGetLastError(void)
{
	return errno;
}


void WSASetLastError(int iError)
{
	errno = iError;
}


int ioctlsocket(int socket, long   cmd, unsigned long *argp)
{
	switch (cmd)
	{
	case FIONBIO:
		if (0 != *argp)
		{
			CRITICAL_ERROR("non blocking socket not implemented yet");
		}
		break;
	default:
		CRITICAL_ERROR("not implemented yet");
		break;
	}
	return 0;
}

ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,
                 struct sockaddr *src_addr, socklen_t *addrlen)
{
	CRITICAL_ERROR("recvfrom not implemented yet");
}
#endif

int select(int nfds, fd_set *readfds, fd_set *writefds,
								fd_set *exceptfds, struct timeval *timeout)
{
	uint8_t i;
	int event_happened;
	fd_set orig_readfds;
	fd_set orig_writefds;
	uint32_t timeout_msec;

#ifdef DEBUG
	{
		uint32_t readfds_for_print;
		uint32_t writefds_for_print;

		if (NULL != readfds)
		{
			readfds_for_print = *(uint32_t*)readfds;
		}
		else
		{
			readfds_for_print = 0;
		}
		if (NULL != writefds)
		{
			writefds_for_print = *(uint32_t*)writefds;
		}
		else
		{
			writefds_for_print = 0;
		}
		PRINTF_DBG("%s: in *(uint32_t*)readfds= %u,"
				"*(uint32_t*)writefds = %u\n",
				__FUNCTION__, readfds_for_print, writefds_for_print);
	}
#endif
	if (NULL != timeout)
	{
		timeout_msec = timeout->tv_sec * 1000;
		timeout_msec += timeout->tv_usec / 1000;
	}
	else
	{
		timeout_msec = 0;
	}

	if ((0 == nfds) &&
			(NULL == readfds) && (NULL == writefds) && (NULL == exceptfds))
	{
		if (0 != timeout_msec)
		{
			os_delay_ms( timeout_msec );
		}
		return 0;
	}

	if (NULL != readfds)
	{
		orig_readfds = *readfds;
		FD_ZERO(readfds);
	}
	if (NULL != writefds)
	{
		orig_writefds = *writefds;
		FD_ZERO(writefds);
	}
	if (NULL != exceptfds)
	{
		FD_ZERO(exceptfds);
	}

	event_happened = 0;
	while (1)
	{
		for(i = 0; i < CONFIG_MAX_NUMBER_OF_OPENED_FILE_DESCRIPTORS; i++)
		{
			if ((NULL != readfds) && ( 0 != FD_ISSET(i, &orig_readfds)))
			{
				struct file_desc_t  *curr_fd;
				uint8_t retVal;
				uint8_t read_ready;

				curr_fd = &file_desc_arr[i];
				retVal = 0;
				read_ready = 0;
				if (curr_fd->used || (NULL == curr_fd->underlying_dev))
				{
					retVal = curr_fd->file_desc_ops.is_data_available_func(
										curr_fd->internal_desc, &read_ready);
				}
				if(0 != retVal)
				{
					PRINTF_DBG(
						"--%s : fd = %d connection was lost\n",__FUNCTION__, i);

					FD_SET(i, exceptfds);
				}
				if (read_ready)
				{
					PRINTF_DBG("%s : readfds\n", __FUNCTION__);
					FD_SET(i, readfds);
					event_happened++;
				}
			}
			if ((NULL != writefds) && ( 0 != FD_ISSET(i, &orig_writefds)))
			{
				PRINTF_DBG("%s : writefds\n", __FUNCTION__);
				FD_SET(i, writefds);
				event_happened++;
			}
		}
		if (event_happened || (0 == timeout_msec))
		{
			break;
		}
		if (NULL != timeout)
		{
			timeout_msec--;
		}
		os_delay_ms( 1 );
	}

	PRINTF_DBG("%s: out  event_happened = %d\n", __FUNCTION__, event_happened);

	return event_happened;// STILL CAN BE BLOCKED IN ESP8266 TASK
	//while(1);//temporary debug trap
}


#if !defined(CONFIG_XTENSA_XCC) && !defined(CONFIG_LINUX_OS_WRAPPER)
struct stat {
	uint8_t dummy;
};

void _stat(const char *path, struct stat *buf)
{
	PRINTF_DBG("%s: path = %s\n", __FUNCTION__, path);
	CRITICAL_ERROR("_stat not implemented yet");
}


int _write(int file, char *ptr, int len)
{
	PRINTF_DBG("%s: fd = %d\n", __FUNCTION__, file);
	CRITICAL_ERROR("_write not implemented yet");
	return len;
}


int _close(int file)
{
	PRINTF_DBG("%s: fd = %d\n", __FUNCTION__, file);
	CRITICAL_ERROR("_close not implemented yet");
	return -1;
}


int _fstat(int file, void *st)
{
	PRINTF_DBG("%s: fd = %d\n", __FUNCTION__, file);
	CRITICAL_ERROR("_fstat not implemented yet");
	return 0;
}


int _isatty(int file)
{
	PRINTF_DBG("%s: fd = %d\n", __FUNCTION__, file);
	CRITICAL_ERROR("_isatty not implemented yet");
	return 1;
}


int _lseek(int file, int ptr, int dir)
{
	PRINTF_DBG("%s: fd = %d\n", __FUNCTION__, file);
	CRITICAL_ERROR("_lseek not implemented yet");
	return 0;
}


int _open(const char *name, int flags, int mode)
{
	PRINTF_DBG("%s :  name = %s\n", __FUNCTION__, name);
	CRITICAL_ERROR("_open not implemented yet");
	return -1;
}


int _read(int file, char *ptr, int len)
{
	PRINTF_DBG("%s: fd = %d\n", __FUNCTION__, file);
	CRITICAL_ERROR("_read not implemented yet");
	return 0;
}
#endif
