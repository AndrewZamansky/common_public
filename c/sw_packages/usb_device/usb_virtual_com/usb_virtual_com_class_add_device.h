/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "usb_virtual_com_class_api.h"
#include  "src/usb_virtual_com_class.h"


#ifndef USB_VIRTUAL_COM_CLASS_DT_CALLBACK_TX_PDEV
	#define _USB_VIRTUAL_COM_CLASS_CALLBACK_TX_PDEV   NULL
#else
	EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(
			USB_VIRTUAL_COM_CLASS_DT_CALLBACK_TX_PDEV);
	#define _USB_VIRTUAL_COM_CLASS_CALLBACK_TX_PDEV     \
		P_TO_STATIC_DEVICE_INST(USB_VIRTUAL_COM_CLASS_DT_CALLBACK_TX_PDEV)
#endif


#ifndef USB_VIRTUAL_COM_CLASS_DT_CALLBACK_RX_PDEV
	#define _USB_VIRTUAL_COM_CLASS_CALLBACK_RX_PDEV   NULL
#else
	EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(
			USB_VIRTUAL_COM_CLASS_DT_CALLBACK_RX_PDEV);
	#define _USB_VIRTUAL_COM_CLASS_CALLBACK_RX_PDEV     \
		P_TO_STATIC_DEVICE_INST(USB_VIRTUAL_COM_CLASS_DT_CALLBACK_RX_PDEV)
#endif


#ifndef USB_VIRTUAL_COM_CLASS_DT_USB_PDEV
	#error "USB_VIRTUAL_COM_CLASS_DT_USB_PDEV should be defined"
#endif
EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(USB_VIRTUAL_COM_CLASS_DT_USB_PDEV);
#define _POINTER_TO_USB_PDEV     \
		P_TO_STATIC_DEVICE_INST(USB_VIRTUAL_COM_CLASS_DT_USB_PDEV)

#ifndef USB_VIRTUAL_COM_CLASS_DT_USB_DESCRIPTOR_PDEV
	#error "USB_VIRTUAL_COM_CLASS_DT_USB_DESCRIPTOR_PDEV should be defined"
#endif
EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(
							USB_VIRTUAL_COM_CLASS_DT_USB_DESCRIPTOR_PDEV);
#define _POINTER_TO_USB_DESCRIPTOR_PDEV     \
	P_TO_STATIC_DEVICE_INST(USB_VIRTUAL_COM_CLASS_DT_USB_DESCRIPTOR_PDEV)

#ifndef USB_VIRTUAL_COM_CLASS_DT_MAX_DATA_PCKT_SIZE
	#error "USB_VIRTUAL_COM_CLASS_DT_MAX_DATA_PCKT_SIZE should be defined"
#endif

SET_STATIC_DEV_CONFIG(usb_virtual_com_class) =
{
	_POINTER_TO_USB_PDEV,
	_USB_VIRTUAL_COM_CLASS_CALLBACK_TX_PDEV,
	_USB_VIRTUAL_COM_CLASS_CALLBACK_RX_PDEV,
	_POINTER_TO_USB_DESCRIPTOR_PDEV,
	USB_VIRTUAL_COM_CLASS_DT_MAX_DATA_PCKT_SIZE
};


#include "add_static_dev.h"

/*
 *  device specific defines should be undefined
 *  after calling #include "add_static_dev.h"
 */
#undef USB_VIRTUAL_COM_CLASS_DT_USB_PDEV
#undef USB_VIRTUAL_COM_CLASS_DT_CALLBACK_TX_PDEV
#undef USB_VIRTUAL_COM_CLASS_DT_CALLBACK_RX_PDEV
#undef _POINTER_TO_USB_PDEV
#undef _USB_VIRTUAL_COM_CLASS_CALLBACK_TX_PDEV
#undef _USB_VIRTUAL_COM_CLASS_CALLBACK_RX_PDEV
#undef USB_VIRTUAL_COM_CLASS_DT_USB_DESCRIPTOR_PDEV
#undef _POINTER_TO_USB_DESCRIPTOR_PDEV
#undef USB_VIRTUAL_COM_CLASS_DT_MAX_DATA_PCKT_SIZE
