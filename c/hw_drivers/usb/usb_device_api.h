
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
	IOCTL_USB_DEVICE_SET_OUT_ENDPOINT,
	IOCTL_USB_DEVICE_ALLOC_ENDPOINTS_NUMBERS
} USB_DEVICE_COMMON_API_IOCTL_T;

enum USB_DEVICE_API_OUT_EP_TYPE_T
{
	USB_DEVICE_API_EP_TYPE_INTERRUPT_OUT,
	USB_DEVICE_API_EP_TYPE_ISO_OUT,
	USB_DEVICE_API_EP_TYPE_BULK_OUT
};

struct set_out_endpoint_t {
	struct dev_desc_t *callback_dev;
	usb_dev_out_endpoint_callback_func_t func;
	uint16_t  max_pckt_size;
	uint8_t   type;
	uint8_t   endpoint_num;
};

#define MAX_NUM_OF_REQUESTED_ENDPOINTS   16
struct usb_device_alloc_endpoints_t {
	uint8_t num_of_endpoints;
	uint8_t endpoints_num[MAX_NUM_OF_REQUESTED_ENDPOINTS];
};

/**********  define API  functions  ************/


#endif
