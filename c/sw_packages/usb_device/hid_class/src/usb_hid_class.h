/*
 *
 * usb_hid_class.h

 *
 */

#ifndef _USB_HID_CLASS_H_
#define _USB_HID_CLASS_H_

#include "dev_management_api.h"

struct usb_hid_class_runtime_t {
	size_t  sentLen;
	uint8_t const *report_desc;
	uint16_t report_desc_size;
	uint8_t in_endpoint_num;
	uint8_t device_started;
	uint8_t hid_report_was_set;
};
SET_RUNTIME_DATA_TYPE(usb_hid_class, struct usb_hid_class_runtime_t);

#endif
