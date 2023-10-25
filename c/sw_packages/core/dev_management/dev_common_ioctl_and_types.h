
#ifndef _dev_common_ioctl_and_types_h_
#define _dev_common_ioctl_and_types_h_



enum common_callback_e {
	CALLBACK_VOID,
	CALLBACK_INTERRUPT_ARRIVED,
	CALLBACK_TX_DONE,
	CALLBACK_DATA_RECEIVED,
	CALLBACK_NEW_DATA_ARRIVED,
	CALLBACK_NEW_DATA_FROM_SOCKET_ARRIVED,
	CALLBACK_BTN_STATE_CHANGED,
	CALLBACK_HEARTBEAT_TICK,
	CALLBACK_USB_AUDIO_CLASS_VOLUME_MUTE_CHANGED,
	CALLBACK_USB_AUDIO_CLASS_HOST_STREAMING_STATE_CHANGED,
	CALLBACK_LAST_COMMON_CALLBACK
};


enum common_ioctl_e {
	IOCTL_VOID ,
	IOCTL_SET_SERVER_DEVICE,
	IOCTL_SET_CALLBACK_DEV,
	IOCTL_SET_TX_DONE_CALLBACK,
	IOCTL_SET_ISR_CALLBACK_DEV,
	IOCTL_ADD_ISR_CALLBACK_DEV,
	IOCTL_GET_AND_LOCK_DATA_BUFFER,
	IOCTL_SET_BYTES_CONSUMED_IN_DATA_BUFFER,
	IOCTL_SET_UNLOCK_DATA_BUFFER,
	IOCTL_DEVICE_START,
	IOCTL_DEVICE_STOP,
	IOCTL_OPEN_SOCKET,
	IOCTL_NET_DEVICE_GET_LOCAL_ADDR,
	IOCTL_NET_DEVICE_GET_HOST_ADDR,
	IOCTL_TEST_PARAMS_STATUS,
	IOCTL_CALL_MAIN_THREAD_FUNCTION_OF_DEVICE,
	IOCTL_LAST_COMMON_IOCTL
};


struct ioctl_get_data_buffer_t
{
	uint8_t *pBufferStart;
	size_t TotalLength;
	uint8_t bufferWasOverflowed;
};


struct ioctl_net_device_get_host_addr_t
{
	char const *name;
	char *addr_str;
	uint16_t addr_str_len;
};


struct ioctl_net_device_get_local_addr_t {
	char *addr_str;
	uint8_t addr_str_len;
	uint16_t  port;
};

typedef void (*tx_done_callback_t)(size_t size);

#endif
