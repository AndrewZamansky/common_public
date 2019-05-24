/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "usb_i94xxx_api.h"
#include  "src/usb_i94xxx.h"


#define    MODULE_NAME                  usb_i94xxx
#define    MODULE_IOCTL_FUNCTION        usb_i94xxx_ioctl
#define MODULE_CONFIG_DATA_STRUCT_TYPE  struct usb_i94xxx_cfg_t

#ifdef DT_DEV_MODULE

	#ifndef USB_I94XXX_DT_SRC_CLOCK_PDEV
		#error "USB_I94XXX_DT_SRC_CLOCK_PDEV should be defined"
	#endif
	EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(USB_I94XXX_DT_SRC_CLOCK_PDEV) ;
	#define _POINTER_TO_SRC_CLOCK_PDEV     \
			P_TO_STATIC_DEVICE_INST(USB_I94XXX_DT_SRC_CLOCK_PDEV)

	#define STATIC_DEV_DATA_STRUCT          \
		{                                   \
			_POINTER_TO_SRC_CLOCK_PDEV   ,   \
		}

#endif

#include "add_component.h"

/*
 *  device specific defines should be undefined
 *  after calling #include "add_static_dev.h"
 */
#undef _POINTER_TO_SRC_CLOCK_PDEV
#undef USB_I94XXX_DT_SRC_CLOCK_PDEV
