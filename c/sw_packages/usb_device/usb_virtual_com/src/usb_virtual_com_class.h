/*
 *
 * usb_virtual_com_class.h

 *
 */

#ifndef _USB_VIRTUAL_COM_CLASS_H_
#define _USB_VIRTUAL_COM_CLASS_H_

#include "dev_management_api.h"

struct usb_virtual_com_class_runtime_t {
	size_t  sentLen;
	uint8_t in_endpoint_num;
	uint8_t tx_on;
	uint8_t init_done;
};
SET_RUNTIME_DATA_TYPE(
		usb_virtual_com_class, struct usb_virtual_com_class_runtime_t);

#endif
