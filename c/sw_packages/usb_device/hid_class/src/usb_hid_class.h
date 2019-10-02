/*
 *
 * usb_hid_class.h

 *
 */

#ifndef _USB_HID_CLASS_H_
#define _USB_HID_CLASS_H_

/********  includes *********************/

#include "dev_management_api.h"

/********  types  *********************/



struct usb_hid_class_runtime_t {
	size_t  sentLen;
	uint8_t in_endpoint_num;
	uint8_t tx_on;
	uint8_t init_done;
};
#endif
