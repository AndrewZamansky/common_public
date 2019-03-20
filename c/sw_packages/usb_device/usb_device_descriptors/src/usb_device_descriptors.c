/******************************************************************************
 * @file     usb_device_descriptors.c
 * @brief
 * @version
 * @date
 *
 * @note
 ******************************************************************************/
/*!<Includes */
#include <string.h>

#include "usb_device_api.h"
#include "usb_device_descriptors.h"
#include "string.h"
#include "stdlib.h"
#include "usb_device_descriptors_api.h"

#define WBVAL(x) (x & 0xFF), ((x >>8) & 0xFF)

#define EP0_MAX_PKT_SIZE    64
/* Define the vendor id and product id */
#define USBD_VID     0x0416
#define USBD_PID     0xDEAD

#define VID_POS      8
#define PID_POS      10
/*----------------------------------------------------------------------------*/
/*!<USB Device Descriptor */
uint8_t gu8DeviceDescriptor[] =
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
	0x01,              /* iManufacture */
	0x02,              /* iProduct */
	0x03,              /* iSerialNumber */
	0x01               /* bNumConfigurations */
};

	
#define WILL_BE_CALC_LATER   0
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

#define DESC_STRING         0x03

/*!<USB Language String Descriptor */
const uint8_t gu8StringLang[4] = 
{
	4,              /* bLength */
	DESC_STRING,    /* bDescriptorType */
	0x09, 0x04
};

/*!<USB Vendor String Descriptor */
const uint8_t gu8VendorStringDesc[16] = 
{
	16,
	DESC_STRING,
	'N', 0, 'u', 0, 'v', 0, 'o', 0, 't', 0, 'o', 0, 'n', 0
};

/*!<USB Product String Descriptor */
const uint8_t gu8ProductStringDesc[] = {
	46,             /* bLength          */
	DESC_STRING,    /* bDescriptorType  */
	'N', 0,
	'u', 0, 
	'v', 0, 
	'o', 0, 
	't', 0,
	'o', 0, 
	'n', 0,
	' ', 0,
	'U', 0,
	'A', 0, 
	'C', 0, 
	'_', 0, 
	'H', 0,
	'I', 0, 
	'D', 0,
	' ', 0,
	'D', 0, 
	'e', 0, 
	'v', 0, 
	'i', 0, 
	'c', 0, 
	'e', 0
};


uint8_t gu8StringSerial[] = 
{
	26,             // bLength
	DESC_STRING,    // bDescriptorType
	'A', 0, '0', 0, '0', 0, '0', 0, '0', 0, '8', 0,
	'0', 0, '4', 0, '0', 0, '1', 0, '1', 0, '5', 0
};

/*!<USB BOS Descriptor */
uint8_t gu8BOSDescriptor[] = 
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


const uint8_t *gpu8UsbString[4] = {
    gu8StringLang,
    gu8VendorStringDesc,
    gu8ProductStringDesc,
    gu8StringSerial
};

#define MAX_INTERFACE_NUM_FOR_HID  6
uint8_t const *gu8UsbHidReport[MAX_INTERFACE_NUM_FOR_HID] = {NULL};
uint32_t gu32UsbHidReportLen[MAX_INTERFACE_NUM_FOR_HID] = {0};
uint32_t gu32ConfigHidDescIdx[MAX_INTERFACE_NUM_FOR_HID] = {0};



static uint8_t *configuration_desc;
uint16_t configuration_desc_size;
uint16_t interface_count;

static void increase_configuration_desc(uint16_t additional_size)
{
	uint16_t new_desc_size;
	new_desc_size = configuration_desc_size + additional_size;
	configuration_desc = (uint8_t*)realloc(configuration_desc, new_desc_size);
	if (NULL == configuration_desc)
	{
		CRITICAL_ERROR("not enough memory in heap\n");
	}
	configuration_desc_size = new_desc_size;
}

static void device_start(struct usb_device_descriptors_cfg_t *cfg_hndl)
{
	struct dev_desc_t *usb_hw;
	uint16_t VID;
	uint16_t PID;

	usb_hw = cfg_hndl->usb_hw;
	interface_count = 0;
	configuration_desc_size = sizeof(gu8ConfigDescriptor);
	configuration_desc = (uint8_t*)malloc(configuration_desc_size);
	VID = cfg_hndl->VID;
	gu8DeviceDescriptor[VID_POS] = VID & 0xff;
	gu8DeviceDescriptor[VID_POS + 1] = (VID >> 8) & 0xff;
	PID = cfg_hndl->PID;
	gu8DeviceDescriptor[PID_POS] = PID & 0xff;
	gu8DeviceDescriptor[PID_POS + 1] = (PID >> 8) & 0xff;
	memcpy(
		configuration_desc, gu8ConfigDescriptor, configuration_desc_size);
	DEV_IOCTL_0_PARAMS(usb_hw, IOCTL_DEVICE_START);
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



/**
 * usb_i94xxx_ioctl()
 *
 * return:
 */
uint8_t usb_device_descriptors_ioctl(
		struct dev_desc_t *adev, uint8_t aIoctl_num,
		void * aIoctl_param1, void * aIoctl_param2)
{
	struct usb_device_descriptors_cfg_t *cfg_hndl;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(adev);
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
	default :
		return 1;
	}
	return 0;
}
