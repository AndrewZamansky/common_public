/*
 *
 * usb_device_descriptors.h

 *
 */

#ifndef _USB_DEVICE_DESCRIPTORS_H_
#define _USB_DEVICE_DESCRIPTORS_H_

struct usb_device_descriptors_runtime_t {
	uint8_t  usb_device_started;
};
SET_RUNTIME_DATA_TYPE(
		usb_device_descriptors, struct usb_device_descriptors_runtime_t);

#endif
