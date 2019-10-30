
#ifndef _USB_HID_CLASS_API_H_
#define _USB_HID_CLASS_API_H_

#include "dev_management_api.h"

enum USB_HID_CLASS_API_ioctl_e {
	IOCTL_USB_HID_START = IOCTL_LAST_COMMON_IOCTL + 1,
	IOCTL_USB_HID_GET_IN_ENDPOINT_SIZE,
	IOCTL_USB_HID_SET_REPORT_DESCRIPTOR
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

struct usb_hid_set_report_descriptor_t {
	uint8_t const *report_desc;
	uint16_t report_desc_size;
};


#endif
