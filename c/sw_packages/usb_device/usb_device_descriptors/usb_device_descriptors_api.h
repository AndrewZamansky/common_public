
#ifndef _USB_DEVICE_DESCRIPTORS_API_H_
#define _USB_DEVICE_DESCRIPTORS_API_H_


#include "dev_management_api.h"


enum USB_DEVICE_DESCRIPTORS_API_ioctl_e {
	USB_DEVICE_DESCRIPTORS_START = IOCTL_LAST_COMMON_IOCTL + 1,
	USB_DEVICE_DESCRIPTORS_ALLOC_INTERFACES_NUMBERS,
	USB_DEVICE_DESCRIPTORS_ADD_INTERFACE,
	USB_DEVICE_DESCRIPTORS_ADD_INTERFACE_ASSOCIATION_DESCRIPTOR,
	USB_DEVICE_DESCRIPTORS_ADD_STRING_DESCRIPTOR
};

enum USB_DEVICE_DESCRIPTORS_API_string_e {
	USB_DEVICE_DESCRIPTORS_VENDOR_STRING,
	USB_DEVICE_DESCRIPTORS_PRODUCT_STRING,
	USB_DEVICE_DESCRIPTORS_SERIAL_STRING,
	USB_DEVICE_DESCRIPTORS_OTHER_STRING,
};

struct usb_descriptors_hid_descriptor_t {
	uint8_t const *report_descriptor;
	uint32_t size;
	uint16_t hid_desc_position_in_config_desc;
};

struct usb_descriptors_add_interface_t {
	uint8_t  *interface_desc;
	uint16_t interface_desc_size;
	uint8_t  *alt_interface_desc;
	uint16_t alt_interface_desc_size;
	uint8_t  is_hid_interface;
	struct usb_descriptors_hid_descriptor_t  hid_descriptor;

};


#define MAX_NUM_OF_REQUESTED_INTERFACES   16
struct usb_descriptors_alloc_interfaces_t {
	uint8_t num_of_interfaces;
	uint8_t interfaces_num[MAX_NUM_OF_REQUESTED_INTERFACES];
};


struct usb_descriptors_add_string_t {
	uint8_t const *string_descriptor;
	uint32_t size_of_descriptor;// additional step to check validity of call
	uint16_t ret_string_index;
	uint8_t type_of_string;
};

// pass  str_desc_s as argument as it will return string index in one of fields
#define USB_DEVICE_DESCRIPTORS_ADD_STRING(                    \
	__usb_device_descriptors_dev, __str_desc_s, string_desc, string_type)   \
	{                                                         \
		struct usb_descriptors_add_string_t  __str_desc_s;     \
		__str_desc_s.type_of_string = string_type;              \
		__str_desc_s.string_descriptor = string_desc;           \
		__str_desc_s.size_of_descriptor = sizeof(string_desc);  \
		DEV_IOCTL_1_PARAMS(__usb_device_descriptors_dev,      \
			USB_DEVICE_DESCRIPTORS_ADD_STRING_DESCRIPTOR, &__str_desc_s);\
	}


struct usb_device_descriptors_cfg_t {
	struct dev_desc_t *usb_hw;
	uint16_t VID;
	uint16_t PID;
};
SET_CONFIG_TYPE(usb_device_descriptors, struct usb_device_descriptors_cfg_t);


#endif
