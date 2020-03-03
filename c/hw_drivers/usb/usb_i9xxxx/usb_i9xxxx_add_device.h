/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include "src/usb_i9xxxx.h"
#include "usb_i9xxxx_api.h"


#ifndef USB_I9XXXX_DT_SRC_CLOCK_PDEV
	#error "USB_I9XXXX_DT_SRC_CLOCK_PDEV should be defined"
#endif
EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(USB_I9XXXX_DT_SRC_CLOCK_PDEV) ;
#define _POINTER_TO_SRC_CLOCK_PDEV     \
				P_TO_STATIC_DEVICE_INST(USB_I9XXXX_DT_SRC_CLOCK_PDEV)

SET_STATIC_DEV_CONFIG(usb_i9xxxx) =
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
#undef USB_I9XXXX_DT_SRC_CLOCK_PDEV
