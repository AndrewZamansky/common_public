/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "usb_hid_class_api.h"
#include  "src/usb_hid_class.h"


#define    MODULE_NAME                  usb_hid_class
#define    MODULE_IOCTL_FUNCTION        usb_hid_class_ioctl
#define    MODULE_PWRITE_FUNCTION       usb_hid_pwrite
#define MODULE_CONFIG_DATA_STRUCT_TYPE  struct usb_hid_class_cfg_t
#define MODULE_RUNTIME_DATA_STRUCT_TYPE struct usb_hid_class_runtime_t

#ifdef DT_DEV_MODULE

	#ifndef USB_HID_CLASS_DT_CALLBACK_TX_PDEV
		#define _USB_HID_CLASS_CALLBACK_TX_PDEV   NULL
	#else
		EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(
				USB_HID_CLASS_DT_CALLBACK_TX_PDEV);
		#define _USB_HID_CLASS_CALLBACK_TX_PDEV     \
			P_TO_STATIC_DEVICE_INST(USB_HID_CLASS_DT_CALLBACK_TX_PDEV)
	#endif


	#ifndef USB_HID_CLASS_DT_CALLBACK_RX_PDEV
		#define _USB_HID_CLASS_CALLBACK_RX_PDEV   NULL
	#else
		EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(
				USB_HID_CLASS_DT_CALLBACK_RX_PDEV);
		#define _USB_HID_CLASS_CALLBACK_RX_PDEV     \
			P_TO_STATIC_DEVICE_INST(USB_HID_CLASS_DT_CALLBACK_RX_PDEV)
	#endif


	#ifndef USB_HID_CLASS_DT_USB_PDEV
		#error "USB_HID_CLASS_DT_USB_PDEV should be defined"
	#endif
	EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(USB_HID_CLASS_DT_USB_PDEV);
	#define _POINTER_TO_USB_PDEV     \
			P_TO_STATIC_DEVICE_INST(USB_HID_CLASS_DT_USB_PDEV)

	#ifndef USB_HID_CLASS_DT_USB_DESCRIPTOR_PDEV
		#error "USB_HID_CLASS_DT_USB_DESCRIPTOR_PDEV should be defined"
	#endif
	EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(
						USB_HID_CLASS_DT_USB_DESCRIPTOR_PDEV);
	#define _POINTER_TO_USB_DESCRIPTOR_PDEV     \
			P_TO_STATIC_DEVICE_INST(USB_HID_CLASS_DT_USB_DESCRIPTOR_PDEV)

	#ifndef USB_HID_CLASS_DT_MAX_DATA_HOST_IN_PCKT_SIZE
		#error "USB_HID_CLASS_DT_MAX_DATA_HOST_IN_PCKT_SIZE should be defined"
	#endif

	#ifndef USB_HID_CLASS_DT_MAX_DATA_HOST_OUT_PCKT_SIZE
		#error "USB_HID_CLASS_DT_MAX_DATA_HOST_OUT_PCKT_SIZE should be defined"
	#endif

	#define STATIC_DEV_DATA_STRUCT           \
		{                                    \
			_POINTER_TO_USB_PDEV ,           \
			_USB_HID_CLASS_CALLBACK_TX_PDEV,  \
			_USB_HID_CLASS_CALLBACK_RX_PDEV,  \
			_POINTER_TO_USB_DESCRIPTOR_PDEV, \
			USB_HID_CLASS_DT_MAX_DATA_HOST_IN_PCKT_SIZE, \
			USB_HID_CLASS_DT_MAX_DATA_HOST_OUT_PCKT_SIZE, \
		}

	#endif

#include "add_component.h"

/*
 *  device specific defines should be undefined
 *  after calling #include "add_static_dev.h"
 */
#undef USB_HID_CLASS_DT_CALLBACK_TX_PDEV
#undef USB_HID_CLASS_DT_CALLBACK_RX_PDEV
#undef _POINTER_TO_USB_PDEV
#undef _USB_HID_CLASS_CALLBACK_PDEV
#undef _USB_HID_CLASS_CALLBACK_TX_PDEV
#undef _USB_HID_CLASS_CALLBACK_RX_PDEV
#undef USB_HID_CLASS_DT_USB_DESCRIPTOR_PDEV
#undef _POINTER_TO_USB_DESCRIPTOR_PDEV
#undef USB_HID_CLASS_DT_MAX_DATA_HOST_IN_PCKT_SIZE
#undef USB_HID_CLASS_DT_MAX_DATA_HOST_OUT_PCKT_SIZE
