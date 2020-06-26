
#ifndef _file_descriptor_manager_api_h_
#define _file_descriptor_manager_api_h_

#include "project_config.h"
#include "_project_typedefs.h"
#include "_project_defines.h"

#include "sys/socket.h"
#include "sys/types.h"

#ifdef  __cplusplus
extern "C" {
#endif


typedef int (*closesocket_func_t)(void *sockfd);
typedef int (*connect_func_t)(
		void *sockfd, const struct sockaddr *addr, unsigned int addrlen);
typedef uint8_t (*is_data_available_func_t)(void *sockfd,
											uint8_t *is_data_available);
typedef ssize_t (*recv_func_t)(void *sockfd, void *buf, size_t len, int flags);
typedef int (*setsockopt_func_t)(void *sockfd, int level,
				int optname, const void *optval, socklen_t optlen);
typedef int (*bind_func_t)(void *sockfd,
				const struct sockaddr *addr, socklen_t addrlen);
typedef int (*getpeername_func_t)(void *sockfd,
						struct sockaddr *addr, socklen_t *addrlen);
typedef ssize_t (*send_func_t)(void *sockfd,
					const void *buffer, size_t length, int flags);
typedef int (*accept_func_t)(
		void *sockfd, struct sockaddr *addr, socklen_t *addrlen);
typedef int (*listen_func_t)(void *sockfd, int backlog);
typedef int (*getsockname_func_t)(
		void *sockfd, struct sockaddr *local_addr, socklen_t *addrlen);

struct file_desc_ops_t {
#ifdef CONFIG_USE_INTERNAL_SOCKETS_IMPLEMENTATION
	closesocket_func_t closesocket_func;
	connect_func_t connect_func;
	recv_func_t   recv_func;
	setsockopt_func_t  setsockopt_func;
	bind_func_t   bind_func;
	listen_func_t listen_func;
	accept_func_t accept_func;
	getpeername_func_t  getpeername_func;
	getsockname_func_t  getsockname_func;
	send_func_t  send_func;
#endif
	is_data_available_func_t  is_data_available_func;
};

struct file_desc_t {
	int socket_family;
	int socket_type;
	int protocol;
	struct dev_desc_t * underlying_dev;
	void *internal_desc;
	struct file_desc_ops_t file_desc_ops;
	uint32_t  socket_options;
	uint8_t used;
};


#ifdef CONFIG_USE_INTERNAL_SOCKETS_IMPLEMENTATION

struct file_descriptor_manager_ioctl_socket_open_t {
	struct file_desc_t*  new_socket_descriptor;
};

struct file_descriptor_manager_ioctl_fill_socket_func_t {
	struct file_desc_t*  new_socket_descriptor;
};

uint8_t file_descriptor_manager_api_init(void);
uint8_t file_descriptor_manager_api_register_INET_device(
										struct dev_desc_t * ap_dev);
uint8_t file_descriptor_manager_api_register_IPC_device(
										struct dev_desc_t * ap_dev);

#endif

#ifdef  __cplusplus
}
#endif

#endif
