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

#include "audio_class.h"
#include "usb_device_api.h"
#include "usb_device_descriptors.h"


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

	

/*!<USB Configure Descriptor */
const uint8_t gu8ConfigDescriptor[] = 
{
    0x09,        // bLength
    0x02,        // bDescriptorType (Configuration)
//    0xC2, 0x00,  // wTotalLength 
    WBVAL(226),  // wTotalLength
    0x04,        // bNumInterfaces 4
    0x01,        // bConfigurationValue
    0x00,        // iConfiguration (String Index)
    0x80,        // bmAttributes
    0x32,        // bMaxPower 100mA

    0x09,        // bLength
    0x04,        // bDescriptorType (Interface)
    0x00,        // bInterfaceNumber 0
    0x00,        // bAlternateSetting
    0x00,        // bNumEndpoints 0
    0x01,        // bInterfaceClass (Audio)
    0x01,        // bInterfaceSubClass (Audio Control)
    0x00,        // bInterfaceProtocol
    0x00,        // iInterface (String Index)

    0x0A,        // bLength
    0x24,        // bDescriptorType (See Next Line)
    0x01,        // bDescriptorSubtype (CS_INTERFACE -> HEADER)
    0x00, 0x01,  // bcdADC 1.00
    0x48, 0x00,  // wTotalLength 72
    0x02,        // binCollection 0x02
    0x01,        // baInterfaceNr 1
    0x02,        // baInterfaceNr 2

    0x0C,        // bLength
    0x24,        // bDescriptorType (See Next Line)
    0x02,        // bDescriptorSubtype (CS_INTERFACE -> INPUT_TERMINAL)
    0x01,        // bTerminalID
    0x01, 0x01,  // wTerminalType (USB Streaming)
    0x00,        // bAssocTerminal
    0x02,        // bNrChannels 2
    0x03, 0x00,  // wChannelConfig (Left and Right Front)
    0x00,        // iChannelNames
    0x00,        // iTerminal

    0x0A,        // bLength
    0x24,        // bDescriptorType (See Next Line)
    0x06,        // bDescriptorSubtype (CS_INTERFACE -> FEATURE_UNIT)
    0x06,        // bUnitID
    0x01,        // bSourceID
    0x01,        // bControlSize 1
    0x01, 0x02,  // bmaControls[0] (Mute)
    0x02, 0x00,  // bmaControls[1] (Volume)

    0x09,        // bLength
    0x24,        // bDescriptorType (See Next Line)
    0x03,        // bDescriptorSubtype (CS_INTERFACE -> OUTPUT_TERMINAL)
    0x03,        // bTerminalID
    0x01, 0x03,  // wTerminalType (Speaker)
    0x00,        // bAssocTerminal
    0x06,        // bSourceID
    0x00,        // iTerminal

    0x0C,        // bLength
    0x24,        // bDescriptorType (See Next Line)
    0x02,        // bDescriptorSubtype (CS_INTERFACE -> INPUT_TERMINAL)
    0x04,        // bTerminalID
    0x01, 0x02,  // wTerminalType (Microphone)
    0x00,        // bAssocTerminal
    0x02,        // bNrChannels 2
    0x03, 0x00,  // wChannelConfig (Left and Right Front)
    0x00,        // iChannelNames
    0x00,        // iTerminal

    0x0A,        // bLength
    0x24,        // bDescriptorType (See Next Line)
    0x06,        // bDescriptorSubtype (CS_INTERFACE -> FEATURE_UNIT)
    0x05,        // bUnitID
    0x04,        // bSourceID
    0x01,        // bControlSize 1
    0x01, 0x02,  // bmaControls[0] (Mute)
    0x02, 0x00,  // bmaControls[1] (Volume)

    0x09,        // bLength
    0x24,        // bDescriptorType (See Next Line)
    0x03,        // bDescriptorSubtype (CS_INTERFACE -> OUTPUT_TERMINAL)
    0x02,        // bTerminalID
    0x01, 0x01,  // wTerminalType (USB Streaming)
    0x00,        // bAssocTerminal
    0x05,        // bSourceID
    0x00,        // iTerminal

    0x09,        // bLength
    0x04,        // bDescriptorType (Interface)
    0x01,        // bInterfaceNumber 1
    0x00,        // bAlternateSetting
    0x00,        // bNumEndpoints 0
    0x01,        // bInterfaceClass (Audio)
    0x02,        // bInterfaceSubClass (Audio Streaming)
    0x00,        // bInterfaceProtocol
    0x00,        // iInterface (String Index)

    0x09,        // bLength
    0x04,        // bDescriptorType (Interface)
    0x01,        // bInterfaceNumber 1
    0x01,        // bAlternateSetting
    0x01,        // bNumEndpoints 1
    0x01,        // bInterfaceClass (Audio)
    0x02,        // bInterfaceSubClass (Audio Streaming)
    0x00,        // bInterfaceProtocol
    0x00,        // iInterface (String Index)

    0x07,        // bLength
    0x24,        // bDescriptorType (See Next Line)
    0x01,        // bDescriptorSubtype (CS_INTERFACE -> AS_GENERAL)
    0x02,        // bTerminalLink
    0x01,        // bDelay 1
    0x01, 0x00,  // wFormatTag (PCM)

    0x0B,        // bLength
    0x24,        // bDescriptorType (See Next Line)
    0x02,        // bDescriptorSubtype (CS_INTERFACE -> FORMAT_TYPE)
    0x01,        // bFormatType 1
    0x02,        // bNrChannels (Stereo)
    0x02,        // bSubFrameSize 2
    0x10,        // bBitResolution 16
    0x01,        // bSamFreqType 1
    B3VAL(REC_RATE),

    0x09,        // bLength
    0x05,        // bDescriptorType (See Next Line)
    0x82,        // bEndpointAddress (IN/D2H)
    0x0D,        // bmAttributes (Isochronous, Sync, Data EP)
    0xC0, 0x00,  // wMaxPacketSize 192
    0x01,        // bInterval 1 (unit depends on device speed)
    0x00,        // bRefresh
    0x00,        // bSyncAddress

    0x07,        // bLength
    0x25,        // bDescriptorType (See Next Line)
    0x01,        // bDescriptorSubtype (CS_ENDPOINT -> EP_GENERAL)
    0x00,        // bmAttributes (None)
    0x00,        // bLockDelayUnits
    0x00, 0x00,  // wLockDelay 0

    0x09,        // bLength
    0x04,        // bDescriptorType (Interface)
    0x02,        // bInterfaceNumber 2
    0x00,        // bAlternateSetting
    0x00,        // bNumEndpoints 0
    0x01,        // bInterfaceClass (Audio)
    0x02,        // bInterfaceSubClass (Audio Streaming)
    0x00,        // bInterfaceProtocol
    0x00,        // iInterface (String Index)

    0x09,        // bLength
    0x04,        // bDescriptorType (Interface)
    0x02,        // bInterfaceNumber 2
    0x01,        // bAlternateSetting
    0x01,        // bNumEndpoints 1
    0x01,        // bInterfaceClass (Audio)
    0x02,        // bInterfaceSubClass (Audio Streaming)
    0x00,        // bInterfaceProtocol
    0x00,        // iInterface (String Index)

    0x07,        // bLength
    0x24,        // bDescriptorType (See Next Line)
    0x01,        // bDescriptorSubtype (CS_INTERFACE -> AS_GENERAL)
    0x01,        // bTerminalLink
    0x01,        // bDelay 1
    0x01, 0x00,  // wFormatTag (PCM)

    0x0B,        // bLength
    0x24,        // bDescriptorType (See Next Line)
    0x02,        // bDescriptorSubtype (CS_INTERFACE -> FORMAT_TYPE)
    0x01,        // bFormatType 1
    0x02,        // bNrChannels (Stereo)
    0x02,        // bSubFrameSize 2
    0x10,        // bBitResolution 16
    0x01,        // bSamFreqType 1
    B3VAL(ALT2_PLAY_RATE),
    //B3VAL(PLAY_RATE),
    //B3VAL(ALT1_PLAY_RATE),
    

    0x09,        // bLength
    0x05,        // bDescriptorType (See Next Line)
    0x03,        // bEndpointAddress (OUT/H2D)
    0x0D,        // bmAttributes (Isochronous, Sync, Data EP)
    0x00, 0x03,  // wMaxPacketSize 768
    0x01,        // bInterval 1 (unit depends on device speed)
    0x00,        // bRefresh
    0x00,        // bSyncAddress

    0x07,        // bLength
    0x25,        // bDescriptorType (See Next Line)
    0x01,        // bDescriptorSubtype (CS_ENDPOINT -> EP_GENERAL)
    0x80,        // bmAttributes (None)
    0x00,        // bLockDelayUnits
    0x00, 0x00,  // wLockDelay 0
    
    /* I/F descr: HID */
    LEN_INTERFACE,  /* bLength */
    DESC_INTERFACE, /* bDescriptorType */
    0x03,           /* bInterfaceNumber */
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

/**
 * usb_i94xxx_ioctl()
 *
 * return:
 */
uint8_t usb_device_descriptors_ioctl( struct dev_desc_t *adev, uint8_t aIoctl_num,
		void * aIoctl_param1, void * aIoctl_param2)
{
	struct usb_device_descriptors_cfg_t *cfg_hndl;
	struct dev_desc_t *usb_hw;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(adev);
	usb_hw = cfg_hndl->usb_hw;
	switch(aIoctl_num)
	{


	case IOCTL_DEVICE_START :
		DEV_IOCTL(usb_hw,
				IOCTL_USB_DEVICE_SET_DEVICE_DESC, gu8DeviceDescriptor);
		DEV_IOCTL(usb_hw,
				IOCTL_USB_DEVICE_SET_CONFIG_DESC, gu8ConfigDescriptor);
		DEV_IOCTL(usb_hw,
				IOCTL_USB_DEVICE_SET_STRING_DESC, gpu8UsbString);
		DEV_IOCTL(usb_hw,
				IOCTL_USB_DEVICE_SET_HID_REPORT_DESC, gu8UsbHidReport);
		DEV_IOCTL(usb_hw,
				IOCTL_USB_DEVICE_SET_BOS_DESC, gu8BOSDescriptor);
		DEV_IOCTL(usb_hw,
				IOCTL_USB_DEVICE_SET_HID_REPORT_LEN, gu32UsbHidReportLen);
		DEV_IOCTL(usb_hw,
				IOCTL_USB_DEVICE_SET_HID_DESC_INDEX, gu32ConfigHidDescIdx);
		DEV_IOCTL_0_PARAMS(usb_hw, IOCTL_DEVICE_START);
		DEV_IOCTL_0_PARAMS(usb_hw, IOCTL_USB_DEVICE_START);
		break;

	default :
		return 1;
	}
	return 0;
}
