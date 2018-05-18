
#ifndef _USB_DEVICE_API_H_
#define _USB_DEVICE_API_H_

#include "dev_management_api.h"

/*****************  defines  **************/


/**********  define API  types ************/
typedef void (*usb_dev_out_endpoint_callback_func_t)(
		struct dev_desc_t *callback_dev, uint8_t const *buff, size_t size);


typedef enum
{
	IOCTL_USB_DEVICE_SET_DEVICE_DESC = IOCTL_LAST_COMMON_IOCTL + 1,
	IOCTL_USB_DEVICE_SET_CONFIG_DESC,
	IOCTL_USB_DEVICE_SET_STRING_DESC,
	IOCTL_USB_DEVICE_SET_HID_REPORT_DESC,
	IOCTL_USB_DEVICE_SET_BOS_DESC,
	IOCTL_USB_DEVICE_SET_HID_REPORT_LEN,
	IOCTL_USB_DEVICE_SET_HID_DESC_INDEX,
	IOCTL_USB_DEVICE_START,
	IOCTL_USB_DEVICE_SET_OUT_ENDPOINT_CALLBACK
}USB_DEVICE_COMMON_API_IOCTL_T;

struct set_out_endpoint_callback_t {
	struct dev_desc_t *callback_dev;
	usb_dev_out_endpoint_callback_func_t func;
	uint8_t  endpoint_num;
};

/**********  define API  functions  ************/


#endif
