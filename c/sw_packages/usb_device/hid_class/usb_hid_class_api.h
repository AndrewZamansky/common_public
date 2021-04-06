
#ifndef _USB_HID_CLASS_API_H_
#define _USB_HID_CLASS_API_H_

#include "dev_management_api.h"

#define USB_INTERFACE_DESCRIPTOR_TYPE 0x04
#define USB_ENDPOINT_DESCRIPTOR_TYPE  0x05
#define USB_HID_DESCRIPTOR_TYPE       0x21

#define  HID_USAGE_PAGE(usage_page)  0x05, usage_page
#define  HID_USAGE_PAGE_GENERIC_DESKTOP   0x01

#define  HID_USAGE(usage)  0x09, usage
#define  HID_USAGE_UNDEFINED   0x00

#define  HID_START_COLLECTION(collection)  0xA1, collection
#define  HID_COLLECTION_APPLICATION   0x01
#define  HID_COLLECTION_TELEPHONY     0x0B

#define  HID_END_COLLECTION()  0xC0

#define  HID_LOGICAL_MINIMUM(min)  0x15, min
#define  HID_LOGICAL_MAXIMUM(max)  0x25, max

#define  HID_USAGE_MINIMUM(min)  0x19, min
#define  HID_USAGE_MAXIMUM(max)  0x29, max

#define  HID_REPORT_COUNT(count)  0x95, count
#define  HID_REPORT_SIZE(size)  0x75, size

#define  HID_INPUT(input)   0x81, input
#define  HID_OUTPUT(input)  0x91, input
#define  HID_INOUT_DATA        (0 << 0)
#define  HID_INOUT_CONSTANT    (1 << 0)
#define  HID_INOUT_ARRAY       (0 << 1)
#define  HID_INOUT_VARIABLE    (1 << 1)
#define  HID_INOUT_ABSOLUTE    (0 << 2)
#define  HID_INOUT_RELATIVE    (1 << 2)


enum USB_HID_CLASS_API_ioctl_e {
	IOCTL_USB_HID_START = IOCTL_LAST_COMMON_IOCTL + 1,
	IOCTL_USB_HID_GET_IN_ENDPOINT_SIZE,
	IOCTL_USB_HID_SET_REPORT_DESCRIPTOR
};


struct usb_hid_set_report_descriptor_t {
	uint8_t const *report_desc;
	uint16_t report_desc_size;
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
