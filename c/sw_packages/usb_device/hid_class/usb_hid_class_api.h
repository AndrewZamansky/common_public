
#ifndef _USB_HID_CLASS_API_H_
#define _USB_HID_CLASS_API_H_

#include "uart_api.h"

/*****************  defines  **************/



/**********  define API  types ************/
typedef enum
{
	IOCTL_USB_HID_START = IOCTL_LAST_COMMON_UART_IOCTL + 1,
	IOCTL_USB_HID_SET_REPORT_DESCRIPTOR
} USB_HID_CLASS_API_ioctl_t;


/**********  define API  functions  ************/

struct usb_hid_set_report_descriptor_t {
	uint8_t const *report_desc;
	uint16_t report_desc_size;
};


#endif
