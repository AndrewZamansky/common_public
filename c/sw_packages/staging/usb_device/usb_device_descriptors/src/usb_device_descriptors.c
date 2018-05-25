/******************************************************************************
 * @file     descriptors.c
 * @brief    I94100 series USBD driver source file
 * @version  1.0.0
 * @date     January, 2018
 *
 * @note
 * Copyright (C) 2016 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
/*!<Includes */
#include <string.h>
#include "I94100.h"
//#include "usbd_audio.h"

#include "usb_device_api.h"
#include "usb_device_descriptors.h"
#include "string.h"
#include "stdlib.h"
#include "usb_device_descriptors_api.h"

#define WBVAL(x) (x&0xFF), ((x >>8) & 0xFF) 
#define B3VAL(x) (x&0xFF), ((x >>8) & 0xFF), ((x >>16) & 0xFF)  


const uint8_t gu8HidReportDescriptor[] =
{
    0x05, 0x01, // USAGE_PAGE (Generic Desktop)
    0x09, 0x00, // USAGE (0)
    0xA1, 0x01, // COLLECTION (Application)
    0x15, 0x00, // LOGICAL_MINIMUM (0)
    0x25, 0xFF, // LOGICAL_MAXIMUM (255)
    0x19, 0x01, // USAGE_MINIMUM (1)
    0x29, 0x08, // USAGE_MAXIMUM (8)
    0x95, 0x40, // REPORT_COUNT (8)
    0x75, 0x08, // REPORT_SIZE (8)
    0x81, 0x02, // INPUT (Data,Var,Abs)
    0x19, 0x01, // USAGE_MINIMUM (1)
    0x29, 0x08, // USAGE_MAXIMUM (8)
    0x91, 0x02, // OUTPUT (Data,Var,Abs)
    0xC0        // END_COLLECTION
};

#define HID_REPORT_DESCRIPTOR_SIZE  sizeof(gu8HidReportDescriptor) 

#define EP0_MAX_PKT_SIZE    64
/* Define the vendor id and product id */
#define USBD_VID     0x0416
#define USBD_PID     0x8230

/*----------------------------------------------------------------------------*/
/*!<USB Device Descriptor */
const uint8_t gu8DeviceDescriptor[] = 
{
    LEN_DEVICE,        /* bLength */
    DESC_DEVICE,       /* bDescriptorType */
    //0x10, 0x01,        /* bcdUSB */
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
static uint8_t gu8ConfigDescriptor[] =
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


#define INT_IN_EP_NUM      0x06
#define INT_OUT_EP_NUM     0x07
#define EP6_MAX_PKT_SIZE    64
#define EP7_MAX_PKT_SIZE    64

static uint8_t hid_interface[] =
{
    /* I/F descr: HID */
    LEN_INTERFACE,  /* bLength */
    DESC_INTERFACE, /* bDescriptorType */
    0x01,           /* bInterfaceNumber */
    0x00,           /* bAlternateSetting */
    0x02,           /* bNumEndpoints */
    0x03,           /* bInterfaceClass */
    0x00,           /* bInterfaceSubClass */
    0x00,           /* bInterfaceProtocol */
    0x00,           /* iInterface */

    /* HID Descriptor */
    LEN_HID,        /* Size of this descriptor in UINT8s. */
    DESC_HID,       /* HID descriptor type. */
    0x10, 0x01,     /* HID Class Spec. release number. */
    0x00,           /* H/W target country. */
    0x01,           /* Number of HID class descriptors to follow. */
    DESC_HID_RPT,   /* Descriptor type. */
    /* Total length of report descriptor. */
    sizeof(gu8HidReportDescriptor) & 0x00FF,
    (sizeof(gu8HidReportDescriptor) & 0xFF00) >> 8,

    /* EP Descriptor: interrupt in. */
    LEN_ENDPOINT,   /* bLength */
    DESC_ENDPOINT,  /* bDescriptorType */
    (INT_IN_EP_NUM | EP_INPUT), /* bEndpointAddress */
    EP_INT,         /* bmAttributes */
    /* wMaxPacketSize */
    EP6_MAX_PKT_SIZE & 0x00FF,
    (EP6_MAX_PKT_SIZE & 0xFF00) >> 8,
    10,        /* bInterval */

    /* EP Descriptor: interrupt out. */
    LEN_ENDPOINT,   /* bLength */
    DESC_ENDPOINT,  /* bDescriptorType */
    (INT_OUT_EP_NUM | EP_OUTPUT),   /* bEndpointAddress */
    EP_INT,         /* bmAttributes */
    /* wMaxPacketSize */
    EP7_MAX_PKT_SIZE & 0x00FF,
    (EP7_MAX_PKT_SIZE & 0xFF00) >> 8,
    10     /* bInterval */
};

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
    'A', 0, '0', 0, '0', 0, '0', 0, '0', 0, '8', 0, '0', 0, '4', 0, '0', 0, '1', 0, '1', 0, '5', 0

};

/*!<USB BOS Descriptor */
uint8_t gu8BOSDescriptor[] = 
{
    LEN_BOS,        /* bLength */
    DESC_BOS,       /* bDescriptorType */
    /* wTotalLength */
    0x0C & 0x00FF,
    (0x0C & 0xFF00) >> 8,
    0x01,           /* bNumDeviceCaps */

    /* Device Capability */
    0x7,            /* bLength */
    DESC_CAPABILITY,/* bDescriptorType */
    CAP_USB20_EXT,  /* bDevCapabilityType */
    0x02, 0x00, 0x00, 0x00  /* bmAttributes */
};


const uint8_t *gpu8UsbString[4] = {
    gu8StringLang,
    gu8VendorStringDesc,
    gu8ProductStringDesc,
    gu8StringSerial
};

const uint8_t *gu8UsbHidReport[6] = {
    NULL,
    NULL,
    NULL,
	gu8HidReportDescriptor,
	NULL,
	NULL 
};

const uint32_t gu32UsbHidReportLen[6] = {
    0,
    0,
    0,
	sizeof(gu8HidReportDescriptor),
    0,
    0
};

const uint32_t gu32ConfigHidDescIdx[4] = {
    0,
    0,
    0,
    (sizeof(gu8ConfigDescriptor) - LEN_ENDPOINT - LEN_HID)
};


//const S_USBD_INFO_T gsInfo =
//{
//    (uint8_t *)gu8DeviceDescriptor,
//    (uint8_t *)gu8ConfigDescriptor,
//    (const uint8_t **)gpu8UsbString,
//    (const uint8_t **)gu8UsbHidReport,
//    (uint8_t *)gu8BOSDescriptor,
//    (uint32_t *)gu32UsbHidReportLen,
//    (uint32_t *)gu32ConfigHidDescIdx
//};


static uint8_t *configuration_desc;
uint16_t configuration_desc_size;
uint16_t interface_count;


static void device_start(struct usb_device_descriptors_cfg_t *cfg_hndl)
{
	struct dev_desc_t *usb_hw;

	usb_hw = cfg_hndl->usb_hw;
	interface_count = 0;
	configuration_desc_size = sizeof(gu8ConfigDescriptor);
	configuration_desc = (uint8_t*)malloc(configuration_desc_size);
	memcpy(
		configuration_desc, gu8ConfigDescriptor, configuration_desc_size);
	DEV_IOCTL_0_PARAMS(usb_hw, IOCTL_DEVICE_START);
}


static void add_interface_association_desc(uint8_t *iad_desc)
{
	uint16_t desc_size;
	uint16_t new_desc_size;

	desc_size = iad_desc[0];

	new_desc_size = configuration_desc_size + desc_size;
	configuration_desc =
			(uint8_t*)realloc(configuration_desc, new_desc_size);
	memcpy(&configuration_desc[configuration_desc_size], iad_desc, desc_size);
	configuration_desc_size = new_desc_size;
}


static void add_interface(
		struct usb_descriptors_add_interface_t *usb_desc_add_interface)
{
	uint16_t interface_structure_size;
	uint16_t new_desc_size;
	uint8_t *interface_desc;

	interface_desc = usb_desc_add_interface->interface_desc;
	interface_structure_size = usb_desc_add_interface->interface_desc_size;

	new_desc_size = configuration_desc_size + interface_structure_size;
	configuration_desc =
			(uint8_t*)realloc(configuration_desc, new_desc_size);
	memcpy(&configuration_desc[configuration_desc_size],
			interface_desc, interface_structure_size);
	configuration_desc_size = new_desc_size;

	interface_desc = usb_desc_add_interface->alt_interface_desc;
	interface_structure_size = usb_desc_add_interface->alt_interface_desc_size;

	if (NULL != interface_desc)
	{
		new_desc_size = configuration_desc_size + interface_structure_size;
		configuration_desc =
				(uint8_t*)realloc(configuration_desc, new_desc_size);
		memcpy(&configuration_desc[configuration_desc_size],
				interface_desc, interface_structure_size);
		configuration_desc_size = new_desc_size;
	}

}


static void usb_device_start(struct usb_device_descriptors_cfg_t *cfg_hndl)
{
	struct dev_desc_t *usb_hw;
	struct usb_descriptors_add_interface_t usb_desc_add_interface;

	usb_hw = cfg_hndl->usb_hw;

	hid_interface[2] = interface_count++;
	usb_desc_add_interface.interface_desc = hid_interface;
	usb_desc_add_interface.interface_desc_size =
								sizeof(hid_interface);
	usb_desc_add_interface.alt_interface_desc = NULL;
	add_interface(&usb_desc_add_interface);

	//set total size :
	configuration_desc[2] = configuration_desc_size & 0xFF;
	configuration_desc[3] = (configuration_desc_size >> 8) & 0xFF;
	configuration_desc[4] = interface_count;

	DEV_IOCTL(usb_hw, IOCTL_USB_DEVICE_SET_DEVICE_DESC, gu8DeviceDescriptor);
	DEV_IOCTL(usb_hw, IOCTL_USB_DEVICE_SET_CONFIG_DESC, configuration_desc);
	DEV_IOCTL(usb_hw, IOCTL_USB_DEVICE_SET_STRING_DESC, gpu8UsbString);
	DEV_IOCTL(usb_hw, IOCTL_USB_DEVICE_SET_HID_REPORT_DESC, gu8UsbHidReport);
	DEV_IOCTL(usb_hw, IOCTL_USB_DEVICE_SET_BOS_DESC, gu8BOSDescriptor);
	DEV_IOCTL(usb_hw, IOCTL_USB_DEVICE_SET_HID_REPORT_LEN, gu32UsbHidReportLen);
	DEV_IOCTL(usb_hw,
			IOCTL_USB_DEVICE_SET_HID_DESC_INDEX, gu32ConfigHidDescIdx);
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
	case USB_DEVICE_DESCRIPTORS_ALLOC_INTERFACES_NUMBERS:
		alloc_interfaces(aIoctl_param1);
		break;
	case USB_DEVICE_DESCRIPTORS_ADD_INTERFACE :
		add_interface(aIoctl_param1);
		break;
	case USB_DEVICE_DESCRIPTORS_ADD_INTERFACE_ASSOCIATION_DESCRIPTOR:
		add_interface_association_desc(aIoctl_param1);
		break;
	case USB_DEVICE_DESCRIPTORS_START:
		usb_device_start(cfg_hndl);
		break;
	default :
		return 1;
	}
	return 0;
}
