
#ifndef _IPC_I96XXX_API_H_
#define _IPC_I96XXX_API_H_


#ifdef  __cplusplus
extern "C" {
#endif

#define IPC_DEBUG_PORT  1
#define IPC_SHELL_PORT  2
#define IPC_AUDIO_PORT  3

enum ipc_i96xxx_err_code_e {
	IPC_I96XXX_NO_ERR = 0,
	IPC_I96XXX_ERR_UNKOWN,
	IPC_I96XXX_ERR_IPC_NOT_READY,
	IPC_I96XXX_ERR_SOCKET_NOT_AVAILABLE,
	IPC_I96XXX_ERR_NO_CONNECTION_PENDING,
	IPC_I96XXX_ERR_SOCKET_NOT_CONNECTED,
	IPC_I96XXX_ERR_NO_FREE_PORT_LEFT,
	IPC_I96XXX_ERR_NO_FREE_SOCKET_LEFT,
	IPC_I96XXX_ERR_WRONG_PORT_NUMBER,
	IPC_I96XXX_ERR_SOCKET_ALREADY_BIND,
	IPC_I96XXX_ERR_LISTEN_PORT_NOT_SET,
	IPC_I96XXX_ERR_WRONG_PORT_IS_USED,
	IPC_I96XXX_ERR_REQUEST_TIMED_OUT,
	IPC_I96XXX_ERR_CANNOT_SEND_BAD_LINK,
	IPC_I96XXX_ERR_CANNOT_CONNECT,
	IPC_I96XXX_ERR_CANNOT_SEND,
	IPC_I96XXX_ERR_CANNOT_CLOSE
};


enum IPC_I96XXX_API_ioctl_e {
	I2S_I96XXX_IS_READY_IOCTL = IOCTL_LAST_COMMON_IOCTL + 1,
};


struct ipc_i96xxx_cfg_t {
	struct dev_desc_t *   timer_dev;
};
SET_CONFIG_TYPE(ipc_i96xxx, struct ipc_i96xxx_cfg_t);


struct __attribute__((packed)) ipc_i96xxx_audio_stream_packet_t
{
	uint8_t *in_buffer;
	uint8_t *out_buffer;
	size_t  buffer_size;
};

#ifdef  __cplusplus
}
#endif

#endif
