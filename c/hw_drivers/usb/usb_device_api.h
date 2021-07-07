
#ifndef _USB_DEVICE_API_H_
#define _USB_DEVICE_API_H_

#include "dev_management_api.h"

enum interface_callback_types_e {
	INTERFACE_CALLBACK_TYPE_STANDARD_SET_INTERFACE,
	INTERFACE_CALLBACK_TYPE_REQUEST,
	INTERFACE_CALLBACK_TYPE_DATA_OUT_FINISHED,
};


typedef void (*usb_dev_out_endpoint_callback_func_t)(
		struct dev_desc_t *callback_dev, uint8_t const *buff, size_t size);

typedef void (*usb_dev_in_endpoint_callback_func_t)(
							struct dev_desc_t *callback_dev);

typedef void (*usb_dev_interface_request_callback_func_t)(
	struct dev_desc_t *callback_dev, uint8_t callback_type, uint8_t *request);

typedef void (*usb_dev_endpoint_request_callback_func_t)(
					struct dev_desc_t *callback_dev, uint8_t *request);


enum USB_DEVICE_COMMON_API_IOCTL_e
{
	IOCTL_USB_DEVICE_SET_DESCRIPTORS = IOCTL_LAST_COMMON_IOCTL + 1,
	IOCTL_USB_DEVICE_START,
	IOCTL_USB_DEVICE_SET_ENDPOINTS,
	IOCTL_USB_DEVICE_REGISTER_INTERFACES,
	IOCTL_USB_DEVICE_SET_REQUEST_IN_BUFFER,
	IOCTL_USB_DEVICE_SET_REQUEST_OUT_BUFFER,
	IOCTL_USB_DEVICE_SET_STALL,
	IOCTL_USB_DEVICE_SENT_DATA_TO_IN_ENDPOINT
};

enum USB_DEVICE_API_OUT_EP_TYPE_e
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
	usb_dev_out_endpoint_callback_func_t   *out_func_arr;
	usb_dev_in_endpoint_callback_func_t   *in_func_arr;
	usb_dev_endpoint_request_callback_func_t   *endpoint_request_callback_func;
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
	uint8_t  *data;
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


typedef void (*usb_device_get_str_desc_func_t)(
			uint16_t str_desc_index, const uint8_t **desc);
struct set_device_descriptors_t
{
	const uint8_t *device_desc;
	const uint8_t *config_desc;
	usb_device_get_str_desc_func_t usb_device_get_str_desc_func;
	const uint8_t **hid_report_desc;
	const uint8_t *BOS_desc;
	const uint32_t *hid_report_size;
	const uint32_t *config_hid_desc_index;
};


#endif
