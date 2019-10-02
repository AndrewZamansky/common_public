/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "usb_device_descriptors_api.h"
#include  "src/usb_device_descriptors.h"


#define MODULE_CONFIG_DATA_STRUCT_TYPE  struct usb_device_descriptors_cfg_t


#ifndef USB_DESC_DT_USB_PDEV
	#error "USB_DESC_DT_USB_PDEV should be defined"
#endif
EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(USB_DESC_DT_USB_PDEV);
#define _POINTER_TO_USB_PDEV  P_TO_STATIC_DEVICE_INST(USB_DESC_DT_USB_PDEV)

#ifndef USB_DESC_DT_USB_VID
	#error "USB_DESC_DT_USB_VID should be defined"
#endif
#ifndef USB_DESC_DT_USB_PID
	#error "USB_DESC_DT_USB_PID should be defined"
#endif
#define STATIC_DEV_DATA_STRUCT   \
	{                            \
		_POINTER_TO_USB_PDEV ,   \
		USB_DESC_DT_USB_VID,     \
		USB_DESC_DT_USB_PID      \
	}


#include "add_static_dev.h"

/*
 *  device specific defines should be undefined
 *  after calling #include "add_static_dev.h"
 */
#undef USB_DESC_DT_USB_PDEV
#undef _POINTER_TO_USB_PDEV
#undef USB_DESC_DT_USB_VID
#undef USB_DESC_DT_USB_PID
