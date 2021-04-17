
#ifndef _USB_HID_CLASS_API_H_
#define _USB_HID_CLASS_API_H_

#include "dev_management_api.h"

#define USB_INTERFACE_DESCRIPTOR_TYPE 0x04
#define USB_ENDPOINT_DESCRIPTOR_TYPE  0x05
#define USB_HID_DESCRIPTOR_TYPE       0x21

#define  HID_USAGE_PAGE(usage_page)  0x05, usage_page
#define  HID_USAGE_PAGE_2BYTES(usage_page)  \
						0x06, (usage_page & 0xff), ((usage_page >> 8) & 0xff)
#define  HID_USAGE_PAGE_GENERIC_DESKTOP   0x01
#define  HID_USAGE_PAGE_TELEPHONY         0x0B
#define  HID_USAGE_PAGE_CONSUMER          0x0C

#define  HID_USAGE(usage)  0x09, usage

// generic page usages
#define  HID_USAGE_UNDEFINED   0x00

// consumer page usages
#define  HID_USAGE_CONSUMER_CONTROL   0x01
#define  HID_USAGE_CONSUMER_PLAYBACK_MUTE      0xE2
#define  HID_USAGE_CONSUMER_VOLUME_INCREMENT   0xE9
#define  HID_USAGE_CONSUMER_VOLUME_DECREMENT   0xEA

// telephony page usages
#define  HID_USAGE_TELEPHONY_HEADSET          0x05
#define  HID_USAGE_TELEPHONY_HOOK_SWITCH      0x20
#define  HID_USAGE_TELEPHONY_LINE_BUSY        0x97

#define  HID_START_COLLECTION(collection)  0xA1, collection
#define  HID_COLLECTION_APPLICATION   0x01

#define  HID_END_COLLECTION()  0xC0

#define  HID_REPORT_ID(id) 0x85, id

#define  HID_LOGICAL_MINIMUM(min)  0x15, min
#define  HID_LOGICAL_MAXIMUM(max)  0x25, max

#define  HID_USAGE_MINIMUM(min)  0x19, min
#define  HID_USAGE_MAXIMUM(max)  0x29, max

#define  HID_REPORT_COUNT(count)  0x95, count
#define  HID_REPORT_SIZE(size)  0x75, size

#define  HID_INPUT(mode)    0x81, mode
#define  HID_OUTPUT(mode)   0x91, mode
#define  HID_FEATURE(mode)  0xB1, mode
#define  HID_FEATURE_2BYTES(mode0, mode1)  0xB2, mode0, mode1
#define  HID_IN_OUT_FEATURE_DATA          (0 << 0)
#define  HID_IN_OUT_FEATURE_CONSTANT      (1 << 0)
#define  HID_IN_OUT_FEATURE_ARRAY         (0 << 1)
#define  HID_IN_OUT_FEATURE_VARIABLE      (1 << 1)
#define  HID_IN_OUT_FEATURE_ABSOLUTE      (0 << 2)
#define  HID_IN_OUT_FEATURE_RELATIVE      (1 << 2)
#define  HID_IN_OUT_FEATURE_PREFFERED     (0 << 5)
#define  HID_IN_OUT_FEATURE_NON_PREFFERED (1 << 5)
#define  HID_IN_OUT_FEATURE_BIT_FIELD   (0 << 0) // bit 8 or 0 in second byte
#define  HID_IN_OUT_FEATURE_BUFFERED    (1 << 0) // bit 8 or 0 in second byte


enum USB_HID_CLASS_API_ioctl_e {
	IOCTL_USB_HID_START = IOCTL_LAST_COMMON_IOCTL + 1,
	IOCTL_USB_HID_GET_IN_ENDPOINT_SIZE,
	IOCTL_USB_HID_SET_REPORT_DESCRIPTOR
};


typedef void (*hid_out_report_over_control_pipe_callback_t)(
							uint16_t report_out_received_buf_size);
typedef void (*hid_in_report_over_control_pipe_callback_t)(
							uint8_t report_id, uint8_t **data, uint16_t *size);

struct usb_hid_set_report_descriptor_t {
	uint8_t const *report_desc;
	uint16_t report_desc_size;
	uint8_t  *report_out_buf;
	uint16_t  report_out_buf_size;
	hid_out_report_over_control_pipe_callback_t
					hid_out_report_over_control_pipe_callback;
	hid_in_report_over_control_pipe_callback_t
					hid_in_report_over_control_pipe_callback;
};


struct usb_hid_class_cfg_t {
	struct dev_desc_t *usb_hw;
	struct dev_desc_t *callback_tx_dev;
	struct dev_desc_t *callback_rx_dev;
	struct dev_desc_t *usb_descriptors_dev;
	uint16_t  max_host_in_data_packet_size;
	uint16_t  max_host_out_data_packet_size;
};
SET_CONFIG_TYPE(usb_hid_class, struct usb_hid_class_cfg_t);

#endif
