/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "usb_device_descriptors_api.h"
#include  "src/usb_device_descriptors.h"


#define    MODULE_NAME                  usb_device_descriptors
#define    MODULE_IOCTL_FUNCTION        usb_device_descriptors_ioctl
#define MODULE_CONFIG_DATA_STRUCT_TYPE  struct usb_device_descriptors_cfg_t

#ifdef DT_DEV_MODULE

	#ifndef USB_DESC_DT_USB_PDEV
		#error "USB_DESC_DT_USB_PDEV should be defined"
	#endif
	#define _POINTER_TO_USB_PDEV     \
			P_TO_STATIC_DEVICE_INST(USB_DESC_DT_USB_PDEV)

	#define STATIC_DEV_DATA_STRUCT          \
		{                                   \
			_POINTER_TO_USB_PDEV ,   \
		}

	#endif

#include "add_component.h"

/*
 *  device specific defines should be undefined
 *  after calling #include "add_static_dev.h"
 */
#undef USB_DESC_DT_USB_PDEV
#undef _POINTER_TO_USB_PDEV
