/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "usb_virtual_com_class_api.h"
#include  "src/usb_virtual_com_class.h"


#define    MODULE_NAME                  usb_virtual_com_class
#define    MODULE_IOCTL_FUNCTION        usb_virtual_com_class_ioctl
#define MODULE_CONFIG_DATA_STRUCT_TYPE  struct usb_virtual_com_class_cfg_t
#define MODULE_RUNTIME_DATA_STRUCT_TYPE struct usb_virtual_com_class_runtime_t

#ifdef DT_DEV_MODULE

	#ifndef USB_VIRTUAL_COM_CLASS_DT_CALLBACK_PDEV
		#define _USB_VIRTUAL_COM_CLASS_CALLBACK_PDEV   NULL
	#else
		EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(
							USB_VIRTUAL_COM_CLASS_DT_CALLBACK_PDEV);
		#define _USB_VIRTUAL_COM_CLASS_CALLBACK_PDEV     \
			P_TO_STATIC_DEVICE_INST(USB_VIRTUAL_COM_CLASS_DT_CALLBACK_PDEV)
	#endif


	#ifndef USB_VIRTUAL_COM_CLASS_DT_ENDPOINT_NUM
		#error "USB_VIRTUAL_COM_CLASS_DT_ENDPOINT_NUM should be defined"
	#endif

	#ifndef USB_VIRTUAL_COM_CLASS_DT_USB_PDEV
		#error "USB_VIRTUAL_COM_CLASS_DT_USB_PDEV should be defined"
	#endif
	#define _POINTER_TO_USB_PDEV     \
			P_TO_STATIC_DEVICE_INST(USB_VIRTUAL_COM_CLASS_DT_USB_PDEV)

	#ifndef USB_VIRTUAL_COM_CLASS_DT_USB_DESCRIPTOR_PDEV
		#error "USB_VIRTUAL_COM_CLASS_DT_USB_DESCRIPTOR_PDEV should be defined"
	#endif
	#define _POINTER_TO_USB_DESCRIPTOR_PDEV     \
			P_TO_STATIC_DEVICE_INST(USB_VIRTUAL_COM_CLASS_DT_USB_DESCRIPTOR_PDEV)

	#define STATIC_DEV_DATA_STRUCT           \
		{                                    \
			_POINTER_TO_USB_PDEV ,           \
			_USB_VIRTUAL_COM_CLASS_CALLBACK_PDEV,  \
			_POINTER_TO_USB_DESCRIPTOR_PDEV, \
			USB_VIRTUAL_COM_CLASS_DT_ENDPOINT_NUM, \
		}

	#endif

#include "add_component.h"

/*
 *  device specific defines should be undefined
 *  after calling #include "add_static_dev.h"
 */
#undef USB_VIRTUAL_COM_CLASS_DT_ENDPOINT_NUM
#undef USB_VIRTUAL_COM_CLASS_DT_USB_PDEV
#undef _POINTER_TO_USB_PDEV
#undef _USB_VIRTUAL_COM_CLASS_CALLBACK_PDEV
#undef USB_VIRTUAL_COM_CLASS_DT_CALLBACK_PDEV
#undef USB_VIRTUAL_COM_CLASS_DT_USB_DESCRIPTOR_PDEV
#undef _POINTER_TO_USB_DESCRIPTOR_PDEV
