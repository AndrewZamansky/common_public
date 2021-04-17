/*
 *
 * usb_hid_class.h

 *
 */

#ifndef _USB_HID_CLASS_H_
#define _USB_HID_CLASS_H_

#include "dev_management_api.h"
#include "usb_hid_class_api.h"

struct usb_hid_class_runtime_t {
	size_t  sentLen;
	uint8_t const *report_desc;
	uint8_t  *report_out_buf;
	hid_out_report_over_control_pipe_callback_t
					hid_out_report_over_control_pipe_callback;
	hid_in_report_over_control_pipe_callback_t
					hid_in_report_over_control_pipe_callback;
	uint16_t report_desc_size;
	uint16_t  report_out_max_buf_size;
	uint16_t  report_out_received_buf_size;
	uint8_t in_endpoint_num;
	uint8_t device_started;
	uint8_t hid_report_was_set;
};
SET_RUNTIME_DATA_TYPE(usb_hid_class, struct usb_hid_class_runtime_t);

#endif
