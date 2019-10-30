/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "usb_i94xxx_api.h"
#include  "src/usb_i94xxx.h"


#ifndef USB_I94XXX_DT_SRC_CLOCK_PDEV
	#error "USB_I94XXX_DT_SRC_CLOCK_PDEV should be defined"
#endif
EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(USB_I94XXX_DT_SRC_CLOCK_PDEV) ;
#define _POINTER_TO_SRC_CLOCK_PDEV     \
				P_TO_STATIC_DEVICE_INST(USB_I94XXX_DT_SRC_CLOCK_PDEV)

SET_STATIC_DEV_CONFIG(usb_i94xxx) =
{
	_POINTER_TO_SRC_CLOCK_PDEV,
};

#define MODULE_HAS_NO_RUNTIME_DATA

#include "add_static_dev.h"

/*
 *  device specific defines should be undefined
 *  after calling #include "add_static_dev.h"
 */
#undef _POINTER_TO_SRC_CLOCK_PDEV
#undef USB_I94XXX_DT_SRC_CLOCK_PDEV
