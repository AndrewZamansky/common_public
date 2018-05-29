
#ifndef _USB_DEVICE_API_H_
#define _USB_DEVICE_API_H_

#include "dev_management_api.h"

/*****************  defines  **************/


/**********  define API  types ************/
typedef void (*usb_dev_out_endpoint_callback_func_t)(
		struct dev_desc_t *callback_dev, uint8_t const *buff, size_t size);

typedef void (*usb_dev_in_endpoint_callback_func_t)(
							struct dev_desc_t *callback_dev);

typedef void (*usb_dev_interface_request_callback_func_t)(
					struct dev_desc_t   *callback_dev, uint8_t *request);


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
	IOCTL_USB_DEVICE_SET_ENDPOINTS,
	IOCTL_USB_DEVICE_REGISTER_INTERFACES,
	IOCTL_USB_DEVICE_SET_REQUEST_IN_BUFFER,
	IOCTL_USB_DEVICE_SET_REQUEST_OUT_BUFFER,
	IOCTL_USB_DEVICE_SET_SATLL,
	IOCTL_USB_DEVICE_SENT_DATA_TO_IN_ENDPOINT
} USB_DEVICE_COMMON_API_IOCTL_T;

enum USB_DEVICE_API_OUT_EP_TYPE_T
{
	USB_DEVICE_API_EP_TYPE_INTERRUPT_OUT,
	USB_DEVICE_API_EP_TYPE_ISO_OUT,
	USB_DEVICE_API_EP_TYPE_BULK_OUT,
	USB_DEVICE_API_EP_TYPE_INTERRUPT_IN,
	USB_DEVICE_API_EP_TYPE_ISO_IN,
	USB_DEVICE_API_EP_TYPE_BULK_IN
};


struct set_endpoints_t {
	uint8_t   num_of_endpoints;
	uint8_t   *endpoints_num_arr;
	usb_dev_out_endpoint_callback_func_t   *func_arr;
	usb_dev_in_endpoint_callback_func_t   *in_func_arr;
	struct dev_desc_t   *callback_dev;
	uint16_t   *max_pckt_sizes;
	uint8_t    *endpoints_type_arr;
};


struct set_data_to_in_endpoint_t {
	uint8_t   endpoint_num;
	uint8_t   const *data;
	size_t    size;
};

struct set_request_in_buffer_t {
	uint8_t   *data;
	size_t    size;
};

struct set_request_out_buffer_t {
	uint8_t   const *data;
	size_t    size;
};

struct register_interface_t {
	uint8_t   interfaces_num;
	usb_dev_interface_request_callback_func_t interface_func;
};

// contains pointer to array of interfaces and corresponding callbacks
struct register_interfaces_t {
	uint8_t num_of_interfaces;
	struct register_interface_t  *register_interface_arr;
	struct dev_desc_t   *callback_dev;
};

/**********  define API  functions  ************/


#endif
