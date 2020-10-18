/******************************************************************************
 * @file     usb_device_descriptors.c
 * @brief
 * @version
 * @date
 *
 * @note
 ******************************************************************************/

#include <string.h>

#include "usb_device_api.h"
#include "usb_device_descriptors.h"
#include "string.h"
#include "stdlib.h"
#include "usb_device_descriptors_api.h"
#include "errors_api.h"
#include "os_wrapper.h"

#define WBVAL(x) (x & 0xFF), ((x >>8) & 0xFF)

#define EP0_MAX_PKT_SIZE    64
/* Define the vendor id and product id */
#define USBD_VID     0xDEAD
#define USBD_PID     0xDEAD

#define DESC_STRING         0x03

#define VID_POS      8
#define PID_POS      10
#define VENDOR_STR_POS      14
#define PRODUCT_STR_POS     15
#define SERIAL_STR_POS      16

#define WILL_BE_CALC_LATER   0

/*----------------------------------------------------------------------------*/
/*!<USB Device Descriptor */
static uint8_t gu8DeviceDescriptor[] =
{
	0x12,        /* bLength */
	0x01,       /* bDescriptorType */
	WBVAL(0x0200),     /* bcdUSB */
	0x00,              /* bDeviceClass */
	0x00,              /* bDeviceSubClass */
	0x00,              /* bDeviceProtocol */
	EP0_MAX_PKT_SIZE,  /* bMaxPacketSize0 */
	WBVAL(USBD_VID),   /* idVendor */
	WBVAL(USBD_PID),   /* idProduct */ 
	0x00, 0x00,        /* bcdDevice */
	WILL_BE_CALC_LATER,  /* iManufacture */
	WILL_BE_CALC_LATER,  /* iProduct */
	WILL_BE_CALC_LATER,  /* iSerialNumber */
	0x01               /* bNumConfigurations */
};

	
/*!<USB Configure Descriptor */
static uint8_t const gu8ConfigDescriptor[] =
{
	0x09,        // bLength
	0x02,        // bDescriptorType (Configuration)
	WBVAL(WILL_BE_CALC_LATER),  // wTotalLength
	WILL_BE_CALC_LATER,        // bNumInterfaces
	0x01,        // bConfigurationValue
	0x00,        // iConfiguration (String Index)
	0x80,        // bmAttributes
	0x32,        // bMaxPower 100mA
};


/*!<USB Language String Descriptor */
static const uint8_t gu8StringLang[4] =
{
	4,              /* bLength */
	DESC_STRING,    /* bDescriptorType */
	0x09, 0x04
};


/*!<USB BOS Descriptor */
static uint8_t gu8BOSDescriptor[] =
{
	0x05,        /* bLength */
	0x0F,       /* bDescriptorType : BOS*/
	/* wTotalLength */
	0x0C & 0x00FF,
	(0x0C & 0xFF00) >> 8,
	0x01,           /* bNumDeviceCaps */

	/* Device Capability */
	0x7,            /* bLength */
	0x10,/* bDescriptorType : capability*/
	0x02,  /* bDevCapabilityType : USB20_EXT*/
	0x02, 0x00, 0x00, 0x00  /* bmAttributes */
};


static uint8_t const **gpu8UsbString = NULL;

#define MAX_INTERFACE_NUM_FOR_HID  6
static uint8_t const *gu8UsbHidReport[MAX_INTERFACE_NUM_FOR_HID] = {NULL};
static uint32_t gu32UsbHidReportLen[MAX_INTERFACE_NUM_FOR_HID] = {0};
static uint32_t gu32ConfigHidDescIdx[MAX_INTERFACE_NUM_FOR_HID] = {0};



static uint8_t *configuration_desc;
static uint16_t configuration_desc_size;
static uint16_t interface_count = 0;
static uint16_t string_index_count = 0;
static uint8_t init_done = 0;

static void increase_configuration_desc(uint16_t additional_size)
{
	uint16_t new_desc_size;
	new_desc_size = configuration_desc_size + additional_size;
	configuration_desc =
			(uint8_t*)os_safe_realloc(configuration_desc, new_desc_size);
	errors_api_check_if_malloc_succeed(configuration_desc);

	configuration_desc_size = new_desc_size;
}

static void device_start(struct usb_device_descriptors_cfg_t *cfg_hndl)
{
	struct dev_desc_t *usb_hw;
	uint16_t VID;
	uint16_t PID;

	usb_hw = cfg_hndl->usb_hw;

	if (1 == init_done)
	{
		CRITICAL_ERROR('usb descriptors already initialized');
	}

	configuration_desc_size = sizeof(gu8ConfigDescriptor);
	configuration_desc = (uint8_t*)os_safe_malloc(configuration_desc_size);
	errors_api_check_if_malloc_succeed(configuration_desc);
	VID = cfg_hndl->VID;
	gu8DeviceDescriptor[VID_POS] = VID & 0xff;
	gu8DeviceDescriptor[VID_POS + 1] = (VID >> 8) & 0xff;
	PID = cfg_hndl->PID;
	gu8DeviceDescriptor[PID_POS] = PID & 0xff;
	gu8DeviceDescriptor[PID_POS + 1] = (PID >> 8) & 0xff;
	memcpy(
		configuration_desc, gu8ConfigDescriptor, configuration_desc_size);

	string_index_count = 1;
	gpu8UsbString = (uint8_t const **)os_safe_malloc(sizeof(uint8_t const *));
	errors_api_check_if_malloc_succeed(gpu8UsbString);
	gpu8UsbString[0] = gu8StringLang;

	DEV_IOCTL_0_PARAMS(usb_hw, IOCTL_DEVICE_START);
	init_done = 1;
}


static void add_interface_association_desc(uint8_t *iad_desc)
{
	uint16_t desc_size;
	uint16_t prev_desc_size;

	desc_size = iad_desc[0];
	prev_desc_size = configuration_desc_size;
	increase_configuration_desc(desc_size);
	memcpy(&configuration_desc[prev_desc_size], iad_desc, desc_size);
}


static void add_interface(
		struct usb_descriptors_add_interface_t *usb_desc_add_interface)
{
	uint16_t interface_structure_size;
	uint8_t *interface_desc;
	uint16_t prev_desc_size;

	interface_desc = usb_desc_add_interface->interface_desc;

	if (1 == usb_desc_add_interface->is_hid_interface)
	{
		uint8_t interface_num;
		struct usb_descriptors_hid_descriptor_t  *hid_descriptor;

		interface_num = interface_desc[2];
		hid_descriptor = &usb_desc_add_interface->hid_descriptor;
		if (MAX_INTERFACE_NUM_FOR_HID <= interface_num)
		{
			CRITICAL_ERROR("interface number to big for HID interface \n");
		}
		gu32ConfigHidDescIdx[interface_num] = configuration_desc_size +
							hid_descriptor->hid_desc_position_in_config_desc;
		gu8UsbHidReport[interface_num] = hid_descriptor->report_descriptor;
		gu32UsbHidReportLen[interface_num] = hid_descriptor->size;
	}

	interface_structure_size = usb_desc_add_interface->interface_desc_size;
	prev_desc_size = configuration_desc_size;
	increase_configuration_desc(interface_structure_size);
	memcpy(&configuration_desc[prev_desc_size],
			interface_desc, interface_structure_size);

	interface_desc = usb_desc_add_interface->alt_interface_desc;
	interface_structure_size = usb_desc_add_interface->alt_interface_desc_size;

	if (NULL != interface_desc)
	{
		prev_desc_size = configuration_desc_size;
		increase_configuration_desc(interface_structure_size);

		memcpy(&configuration_desc[prev_desc_size],
				interface_desc, interface_structure_size);
	}
}


static void usb_device_start(struct usb_device_descriptors_cfg_t *cfg_hndl)
{
	struct dev_desc_t *usb_hw;
	struct set_device_descriptors_t descriptors;

	usb_hw = cfg_hndl->usb_hw;

	if ((WILL_BE_CALC_LATER == gu8DeviceDescriptor[VENDOR_STR_POS]) ||
			(WILL_BE_CALC_LATER == gu8DeviceDescriptor[PRODUCT_STR_POS]) ||
			(WILL_BE_CALC_LATER == gu8DeviceDescriptor[SERIAL_STR_POS]) ||
			(0 == interface_count))
	{
		CRITICAL_ERROR('device descriptors not fully initialized yet');
	}

	//set total size :
	configuration_desc[2] = configuration_desc_size & 0xFF;
	configuration_desc[3] = (configuration_desc_size >> 8) & 0xFF;
	configuration_desc[4] = interface_count;

	descriptors.device_desc = gu8DeviceDescriptor;
	descriptors.config_desc = configuration_desc;
	descriptors.pointers_to_strings_descs = gpu8UsbString;
	descriptors.BOS_desc = gu8BOSDescriptor;
	descriptors.hid_report_desc = gu8UsbHidReport;
	descriptors.hid_report_size = gu32UsbHidReportLen;
	descriptors.config_hid_desc_index = gu32ConfigHidDescIdx;
	DEV_IOCTL_1_PARAMS(usb_hw, IOCTL_USB_DEVICE_SET_DESCRIPTORS, &descriptors);
	DEV_IOCTL_0_PARAMS(usb_hw, IOCTL_USB_DEVICE_START);

}


static void alloc_interfaces(
		struct usb_descriptors_alloc_interfaces_t *usb_desc_alloc_interfaces)
{
	int i = 0;

	while (i < usb_desc_alloc_interfaces->num_of_interfaces)
	{
		usb_desc_alloc_interfaces->interfaces_num[i++] = interface_count++;
		if (i == MAX_NUM_OF_REQUESTED_INTERFACES)
		{
			CRITICAL_ERROR("too many interfaces were requested \n");
		}
	}
}


static void add_string_descriptor(struct usb_device_descriptors_cfg_t *cfg_hndl,
				struct usb_descriptors_add_string_t *usb_descriptors_add_string)
{
	uint8_t const *string_descriptor;
	uint32_t size_of_descriptor;// additional step to check validity of call
	uint16_t curr_string_index_count;

	string_descriptor = usb_descriptors_add_string->string_descriptor;
	size_of_descriptor = usb_descriptors_add_string->size_of_descriptor;
	if (string_descriptor[0] != size_of_descriptor)
	{
		CRITICAL_ERROR('bad size of string descriptor');
	}
	if (string_descriptor[1] != DESC_STRING)
	{
		CRITICAL_ERROR('bad string descriptor');
	}

	curr_string_index_count = string_index_count;
	string_index_count++;
	gpu8UsbString = (uint8_t const **)os_safe_realloc(gpu8UsbString,
								string_index_count * sizeof(uint8_t const *));
	errors_api_check_if_malloc_succeed(gpu8UsbString);

	gpu8UsbString[curr_string_index_count] = string_descriptor;
	usb_descriptors_add_string->ret_string_index = curr_string_index_count;

	switch (usb_descriptors_add_string->type_of_string)
	{
	case USB_DEVICE_DESCRIPTORS_VENDOR_STRING:
		gu8DeviceDescriptor[VENDOR_STR_POS] = curr_string_index_count;
		break;
	case USB_DEVICE_DESCRIPTORS_PRODUCT_STRING:
		gu8DeviceDescriptor[PRODUCT_STR_POS] = curr_string_index_count;
		break;
	case USB_DEVICE_DESCRIPTORS_SERIAL_STRING:
		gu8DeviceDescriptor[SERIAL_STR_POS] = curr_string_index_count;
		break;
	default:
		break;
	}
}


/**
 * usb_i94xxx_ioctl()
 *
 * return:
 */
static uint8_t usb_device_descriptors_ioctl(
		struct dev_desc_t *adev, uint8_t aIoctl_num,
		void * aIoctl_param1, void * aIoctl_param2)
{
	struct usb_device_descriptors_cfg_t *cfg_hndl;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(usb_device_descriptors, adev);

	if ((0 == init_done) && (IOCTL_DEVICE_START != aIoctl_num))
	{
		CRITICAL_ERROR('usb descriptors not initialized');
	}

	switch(aIoctl_num)
	{
	case IOCTL_DEVICE_START :
		device_start(cfg_hndl);
		break;
	case USB_DEVICE_DESCRIPTORS_ALLOC_INTERFACES_NUMBERS :
		alloc_interfaces(aIoctl_param1);
		break;
	case USB_DEVICE_DESCRIPTORS_ADD_INTERFACE :
		add_interface(aIoctl_param1);
		break;
	case USB_DEVICE_DESCRIPTORS_ADD_INTERFACE_ASSOCIATION_DESCRIPTOR :
		add_interface_association_desc(aIoctl_param1);
		break;
	case USB_DEVICE_DESCRIPTORS_START :
		usb_device_start(cfg_hndl);
		break;
	case USB_DEVICE_DESCRIPTORS_ADD_STRING_DESCRIPTOR :
		add_string_descriptor(cfg_hndl, aIoctl_param1);
		break;
	default :
		return 1;
	}
	return 0;
}

#define MODULE_NAME             usb_device_descriptors
#define MODULE_IOCTL_FUNCTION   usb_device_descriptors_ioctl
#define MODULE_HAS_NO_RUNTIME_DATA
#include "add_module.h"
