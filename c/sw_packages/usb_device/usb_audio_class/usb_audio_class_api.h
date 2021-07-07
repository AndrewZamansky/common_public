
#ifndef _USB_AUDIO_CLASS_API_H_
#define _USB_AUDIO_CLASS_API_H_

#include "dev_management_api.h"

#define USB_AUDIO_CLASS_NO_PLAYBACK                      0
#define USB_AUDIO_CLASS_SYNC_WITH_CORRECTIONS_PLAYBACK   1
#define USB_AUDIO_CLASS_ASYNC_PLAYBACK                   2

#define USB_AUDIO_CLASS_BIT_CONTROL_MUTE     0x1
#define USB_AUDIO_CLASS_BIT_CONTROL_VOLUME   0x2

#define USB_AUDIO_CLASS_IN_TERMINAL_TYPE__MIC       0x0201
#define USB_AUDIO_CLASS_IN_TERMINAL_TYPE__HANDSET   0x0401
#define USB_AUDIO_CLASS_IN_TERMINAL_TYPE__HEADSET   0x0402
#define USB_AUDIO_CLASS_IN_TERMINAL_TYPE__SPKPHONE_NO_ECHO_SUPPR     0x0403
#define USB_AUDIO_CLASS_IN_TERMINAL_TYPE__SPKPHONE_ECHO_SUPPR    0x0404
#define USB_AUDIO_CLASS_IN_TERMINAL_TYPE__SPKPHONE_ECHO_CANCEL   0x0405

#define USB_AUDIO_CLASS_OUT_TERMINAL_TYPE__SPK       0x0301
#define USB_AUDIO_CLASS_OUT_TERMINAL_TYPE__HANDSET   0x0401
#define USB_AUDIO_CLASS_OUT_TERMINAL_TYPE__HEADSET   0x0402
#define USB_AUDIO_CLASS_OUT_TERMINAL_TYPE__SPKPHONE_NO_ECHO_SUPPR     0x0403
#define USB_AUDIO_CLASS_OUT_TERMINAL_TYPE__SPKPHONE_ECHO_SUPPR    0x0404
#define USB_AUDIO_CLASS_OUT_TERMINAL_TYPE__SPKPHONE_ECHO_CANCEL   0x0405

enum USB_AUDIO_CLASS_API__callback_e {
	USB_AUDIO_CLASS_CALLBACK_VOLUME_CHANGED = CALLBACK_LAST_COMMON_IOCTL,
	USB_AUDIO_CLASS_CALLBACK_MUTE_CHANGED
};


enum USB_AUDIO_CLASS_API_ioctl_e {
	USB_AUDIO_CLASS_IOCTL_GET_RX_BUFF = IOCTL_LAST_COMMON_IOCTL + 1,
	USB_AUDIO_CLASS_IOCTL_RELEASE_RX_BUFF,
	USB_AUDIO_CLASS_IOCTL_GET_EMPTY_TX_BUFF,
	USB_AUDIO_CLASS_IOCTL_RELEASE_TX_BUFF,
	USB_AUDIO_CLASS_IOCTL_SLOWDOWN_BY_SKIPING_SAMPLES,
	USB_AUDIO_CLASS_IOCTL_ACCELERATE_BY_REPEATING_SAMPLES,
	USB_AUDIO_CLASS_IOCTL_SOFT_RESET,
	USB_AUDIO_CLASS_IOCTL_GET_MASTER_VOLUMES,
	USB_AUDIO_CLASS_IOCTL_GET_MASTER_MUTES
};

struct usb_audio_class_cfg_t {
	struct dev_desc_t *usb_hw;
	struct dev_desc_t *control_callback_dev;
	struct dev_desc_t *usb_descriptors_dev;

	// USB HOST AUDIO IN
	size_t   get_tx_buff_size;
	uint32_t host_in_sample_rate_hz;
	uint16_t in_terminal_type;
	uint8_t  num_of_tx_bytes_per_sample;
	uint8_t  num_tx_channels;
	uint8_t  host_in_master_control_bit_map;
	uint8_t  host_in_channel_control_bit_map;
	uint8_t  enable_recording;

	// USB HOST AUDIO OUT
	size_t   get_rx_buff_size;
	uint32_t host_out_sample_rate_hz;
	uint16_t out_terminal_type;
	uint8_t  num_of_rx_bytes_per_sample;
	uint8_t  num_rx_channels;
	uint8_t  num_of_rx_buffers;
	uint8_t  num_of_rx_over_under_flow_buffers_thr;
	uint8_t  host_out_master_control_bit_map;
	uint8_t  host_out_channel_control_bit_map;
	uint8_t  playback_type;
};
SET_CONFIG_TYPE(usb_audio_class, struct usb_audio_class_cfg_t);

#endif
