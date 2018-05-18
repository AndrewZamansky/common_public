/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "usb_audio_class_api.h"
#include  "src/usb_audio_class.h"


#define    MODULE_NAME                  usb_audio_class
#define    MODULE_IOCTL_FUNCTION        usb_audio_class_ioctl
#define MODULE_CONFIG_DATA_STRUCT_TYPE  struct usb_audio_class_cfg_t
#define MODULE_RUNTIME_DATA_STRUCT_TYPE struct usb_audio_class_runtime_t

#ifdef DT_DEV_MODULE

	#ifndef USB_AUDIO_CLASS_DT_CALLBACK_PDEV
		#define _USB_AUDIO_CLASS_CALLBACK_PDEV   NULL
	#else
		EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(USB_AUDIO_CLASS_DT_CALLBACK_PDEV);
		#define _USB_AUDIO_CLASS_CALLBACK_PDEV     \
			P_TO_STATIC_DEVICE_INST(USB_AUDIO_CLASS_DT_CALLBACK_PDEV)
	#endif


	#ifndef USB_AUDIO_CLASS_DT_ENDPOINT_NUM
		#error "USB_AUDIO_CLASS_DT_ENDPOINT_NUM should be defined"
	#endif

	#ifndef USB_AUDIO_CLASS_DT_NUM_OF_BYTES_PER_SAMPLE
		#error "USB_AUDIO_CLASS_DT_NUM_OF_BYTES_PER_SAMPLE should be defined"
	#endif

	#ifndef USB_AUDIO_CLASS_DT_USB_PDEV
		#error "USB_AUDIO_CLASS_DT_USB_PDEV should be defined"
	#endif
	#define _POINTER_TO_USB_PDEV     \
			P_TO_STATIC_DEVICE_INST(USB_AUDIO_CLASS_DT_USB_PDEV)

	#ifndef USB_AUDIO_CLASS_DT_BUFFER_SIZE
		#error "USB_AUDIO_CLASS_DT_BUFFER_SIZE should be defined"
	#endif

	#define STATIC_DEV_DATA_STRUCT           \
		{                                    \
			USB_AUDIO_CLASS_DT_BUFFER_SIZE,  \
			_POINTER_TO_USB_PDEV ,           \
			_USB_AUDIO_CLASS_CALLBACK_PDEV,  \
			USB_AUDIO_CLASS_DT_ENDPOINT_NUM, \
			USB_AUDIO_CLASS_DT_NUM_OF_BYTES_PER_SAMPLE \
		}

	#endif

#include "add_component.h"

/*
 *  device specific defines should be undefined
 *  after calling #include "add_static_dev.h"
 */
#undef USB_AUDIO_CLASS_DT_ENDPOINT_NUM
#undef USB_AUDIO_CLASS_DT_USB_PDEV
#undef _POINTER_TO_USB_PDEV
#undef _USB_AUDIO_CLASS_CALLBACK_PDEV
#undef USB_AUDIO_CLASS_DT_CALLBACK_PDEV
#undef USB_AUDIO_CLASS_DT_BUFFER_SIZE
#undef USB_AUDIO_CLASS_DT_NUM_OF_BYTES_PER_SAMPLE
