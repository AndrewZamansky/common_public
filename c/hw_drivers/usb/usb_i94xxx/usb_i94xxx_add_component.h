/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "usb_i94xxx_api.h"
#include     "src/usb_i94xxx.h"


#define    MODULE_NAME                  usb_i94xxx
#define    MODULE_IOCTL_FUNCTION        usb_i94xxx_ioctl
//#define    MODULE_CALLBACK_FUNCTION     usb_i94xxx_callback
//#define    MODULE_PWRITE_FUNCTION       usb_i94xxx_pwrite
//#define MODULE_CONFIG_DATA_STRUCT_TYPE  struct usb_i94xxx_cfg_t

#ifdef DT_DEV_MODULE

//	#ifdef USB_I94XXX_DT_TX_CALLBACK_PDEV
//		EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(
//							USB_I94XXX_DT_TX_CALLBACK_PDEV) ;
//		#define POINTER_TO_TX_CALLBACK_PDEV        \
//				P_TO_STATIC_DEVICE_INST(USB_I94XXX_DT_TX_CALLBACK_PDEV)
//	#else
//		#warning "USB_I94XXX_DT_TX_CALLBACK_PDEV not defined in device tree"
//		#define POINTER_TO_TX_CALLBACK_PDEV        NULL
//	#endif
//
//	#ifdef USB_I94XXX_DT_RX_CALLBACK_PDEV
//		EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(
//							USB_I94XXX_DT_RX_CALLBACK_PDEV) ;
//		#define POINTER_TO_RX_CALLBACK_PDEV        \
//		P_TO_STATIC_DEVICE_INST(USB_I94XXX_DT_RX_CALLBACK_PDEV)
//	#else
//		#warning "USB_I94XXX_DT_RX_CALLBACK_PDEV not defined in device tree"
//		#define POINTER_TO_RX_CALLBACK_PDEV        NULL
//	#endif
//
//
//
//	#define STATIC_DEV_DATA_STRUCT          \
//		{                                   \
//			POINTER_TO_TX_CALLBACK_PDEV ,   \
//			POINTER_TO_RX_CALLBACK_PDEV ,   \
//		}
//
	#endif

#include "add_component.h"

/*
 *  device specific defines should be undefined
 *  after calling #include "add_static_dev.h"
 */
#undef USB_I94XXX_DT_BASE_ADDRESS
#undef POINTER_TO_SRC_CLOCK_PDEV
#undef POINTER_TO_TX_CALLBACK_PDEV
#undef POINTER_TO_RX_CALLBACK_PDEV
#undef USB_I94XXX_DT_USB_NUMBER
#undef USB_I94XXX_DT_CALLBACK_PDEV
#undef USB_I94XXX_DT_BAUD_RATE
#undef USB_I94XXX_DT_SRC_CLOCK_PDEV
