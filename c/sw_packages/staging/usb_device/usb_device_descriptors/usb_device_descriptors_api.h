
#ifndef _USB_DEVICE_DESCRIPTORS_API_H_
#define _USB_DEVICE_DESCRIPTORS_API_H_


/*****************  defines  **************/



/**********  define API  types ************/


typedef enum
{
	USB_DEVICE_DESCRIPTORS_START = IOCTL_LAST_COMMON_IOCTL + 1,
	USB_DEVICE_DESCRIPTORS_ALLOC_INTERFACES_NUMBERS,
	USB_DEVICE_DESCRIPTORS_ADD_INTERFACE,
	USB_DEVICE_DESCRIPTORS_ADD_INTERFACE_ASSOCIATION_DESCRIPTOR
} USB_DEVICE_DESCRIPTORS_API_ioctl_t;

struct usb_descriptors_add_interface_t {
	uint8_t *interface_desc;
	uint16_t interface_desc_size;
	uint8_t *alt_interface_desc;
	uint16_t alt_interface_desc_size;
};


#define MAX_NUM_OF_REQUESTED_INTERFACES   16
struct usb_descriptors_alloc_interfaces_t {
	uint8_t num_of_interfaces;
	uint8_t interfaces_num[MAX_NUM_OF_REQUESTED_INTERFACES];
};

/**********  define API  functions  ************/

#endif
