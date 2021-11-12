/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "usb_device_descriptors_api.h"
#include  "src/usb_device_descriptors.h"

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

#ifndef USB_DESC_DT_MAX_CURRENT_mA
	#define USB_DESC_DT_MAX_CURRENT_mA  100
#endif
#if (USB_DESC_DT_MAX_CURRENT_mA > 500)
	#error "maximal allowed current is 500mA"
#endif
//convert to 2mA scale according to USB standard:
#define __USB_DESC_DT_MAX_CURRENT_2mA  (USB_DESC_DT_MAX_CURRENT_mA / 2)

#ifndef USB_DESC_DT_IS_SELF_POWERED
	#define USB_DESC_DT_IS_SELF_POWERED  0
#endif

SET_STATIC_DEV_CONFIG(usb_device_descriptors) =
{
	_POINTER_TO_USB_PDEV,
	USB_DESC_DT_USB_VID,
	USB_DESC_DT_USB_PID,
	__USB_DESC_DT_MAX_CURRENT_2mA,
	USB_DESC_DT_IS_SELF_POWERED
};

#include "add_static_dev.h"

/*
 *  device specific defines should be undefined
 *  after calling #include "add_static_dev.h"
 */
#undef USB_DESC_DT_USB_PDEV
#undef _POINTER_TO_USB_PDEV
#undef USB_DESC_DT_USB_VID
#undef USB_DESC_DT_USB_PID
#undef USB_DESC_DT_MAX_CURRENT_mA
#undef __USB_DESC_DT_MAX_CURRENT_2mA
#undef USB_DESC_DT_IS_SELF_POWERED
