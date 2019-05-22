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
		EXTERN_DECLARATION_TO_STATIC_DEVICE_INST
						(USB_AUDIO_CLASS_DT_CALLBACK_PDEV);
		#define _USB_AUDIO_CLASS_CALLBACK_PDEV     \
			P_TO_STATIC_DEVICE_INST(USB_AUDIO_CLASS_DT_CALLBACK_PDEV)
	#endif

	#ifndef USB_AUDIO_CLASS_DT_USB_PDEV
		#error "USB_AUDIO_CLASS_DT_USB_PDEV should be defined"
	#endif
	EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(USB_AUDIO_CLASS_DT_USB_PDEV);
	#define _POINTER_TO_USB_PDEV     \
			P_TO_STATIC_DEVICE_INST(USB_AUDIO_CLASS_DT_USB_PDEV)

	#ifndef USB_AUDIO_CLASS_DT_USB_DESCRIPTOR_PDEV
		#error "USB_AUDIO_CLASS_DT_USB_DESCRIPTOR_PDEV should be defined"
	#endif
	EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(
					USB_AUDIO_CLASS_DT_USB_DESCRIPTOR_PDEV);
	#define _POINTER_TO_USB_DESCRIPTOR_PDEV     \
			P_TO_STATIC_DEVICE_INST(USB_AUDIO_CLASS_DT_USB_DESCRIPTOR_PDEV)

	#ifndef USB_AUDIO_CLASS_DT_ENABLE_RECORDING
		#error "USB_AUDIO_CLASS_DT_ENABLE_RECORDING should be defined"
	#endif

#if (0 != USB_AUDIO_CLASS_DT_ENABLE_RECORDING)
	#ifndef USB_AUDIO_CLASS_DT_GET_TX_BUFFER_SIZE
		#error "USB_AUDIO_CLASS_DT_GET_TX_BUFFER_SIZE should be defined"
	#endif

	#ifndef USB_AUDIO_CLASS_DT_NUM_OF_TX_BYTES_PER_SAMPLE
		#error "USB_AUDIO_CLASS_DT_NUM_OF_TX_BYTES_PER_SAMPLE should be defined"
	#endif

	#ifndef USB_AUDIO_CLASS_DT_NUM_OF_TX_CHANNELS
		#error "USB_AUDIO_CLASS_DT_NUM_OF_TX_CHANNELS should be defined"
	#endif

	#ifndef USB_AUDIO_CLASS_DT_HOST_IN_SAMPLE_RATE_HZ
		#error "USB_AUDIO_CLASS_DT_HOST_IN_SAMPLE_RATE_HZ should be defined"
	#endif
#else
	#undef   USB_AUDIO_CLASS_DT_GET_TX_BUFFER_SIZE
	#define  USB_AUDIO_CLASS_DT_GET_TX_BUFFER_SIZE  0

	#undef   USB_AUDIO_CLASS_DT_NUM_OF_TX_BYTES_PER_SAMPLE
	#define  USB_AUDIO_CLASS_DT_NUM_OF_TX_BYTES_PER_SAMPLE  0

	#undef   USB_AUDIO_CLASS_DT_NUM_OF_TX_CHANNELS
	#define  USB_AUDIO_CLASS_DT_NUM_OF_TX_CHANNELS  0

	#undef   USB_AUDIO_CLASS_DT_HOST_IN_SAMPLE_RATE_HZ
	#define  USB_AUDIO_CLASS_DT_HOST_IN_SAMPLE_RATE_HZ  0
#endif

	#ifndef USB_AUDIO_CLASS_DT_PLAYBACK_TYPE
		#error "USB_AUDIO_CLASS_DT_PLAYBACK_TYPE should be defined"
	#endif

#if (USB_AUDIO_CLASS_NO_PLAYBACK != USB_AUDIO_CLASS_DT_PLAYBACK_TYPE)
	#ifndef USB_AUDIO_CLASS_DT_GET_RX_BUFFER_SIZE
		#error "USB_AUDIO_CLASS_DT_GET_RX_BUFFER_SIZE should be defined"
	#endif

	#ifndef USB_AUDIO_CLASS_DT_NUM_OF_RX_BYTES_PER_SAMPLE
		#error "USB_AUDIO_CLASS_DT_NUM_OF_RX_BYTES_PER_SAMPLE should be defined"
	#endif

	#ifndef USB_AUDIO_CLASS_DT_HOST_OUT_SAMPLE_RATE_HZ
		#error "USB_AUDIO_CLASS_DT_HOST_OUT_SAMPLE_RATE_HZ should be defined"
	#endif

	#ifndef USB_AUDIO_CLASS_DT_NUM_OF_RX_CHANNELS
		#error "USB_AUDIO_CLASS_DT_NUM_OF_RX_CHANNELS should be defined"
	#endif

	#ifndef USB_AUDIO_CLASS_DT_NUM_OF_RX_BUFFERS
		#error "USB_AUDIO_CLASS_DT_NUM_OF_RX_BUFFERS should be defined"
	#endif

	#ifndef USB_AUDIO_CLASS_DT_NUM_OF_RX_OVER_UNDER_FLOW_BUFFERS_THR
		#error "USB_AUDIO_CLASS_DT_NUM_OF_RX_OVER_UNDER_FLOW_BUFFERS_THR should be defined"
	#endif
#else
	#undef   USB_AUDIO_CLASS_DT_GET_RX_BUFFER_SIZE
	#define  USB_AUDIO_CLASS_DT_GET_RX_BUFFER_SIZE  0

	#undef   USB_AUDIO_CLASS_DT_HOST_OUT_SAMPLE_RATE_HZ
	#define  USB_AUDIO_CLASS_DT_HOST_OUT_SAMPLE_RATE_HZ  0

	#undef   USB_AUDIO_CLASS_DT_NUM_OF_RX_CHANNELS
	#define  USB_AUDIO_CLASS_DT_NUM_OF_RX_CHANNELS  0

	#undef   USB_AUDIO_CLASS_DT_NUM_OF_RX_BUFFERS
	#define  USB_AUDIO_CLASS_DT_NUM_OF_RX_BUFFERS  0

	#undef   USB_AUDIO_CLASS_DT_NUM_OF_RX_OVER_UNDER_FLOW_BUFFERS_THR
	#define  USB_AUDIO_CLASS_DT_NUM_OF_RX_OVER_UNDER_FLOW_BUFFERS_THR  0
#endif

	#define STATIC_DEV_DATA_STRUCT           \
		{                                    \
			USB_AUDIO_CLASS_DT_GET_RX_BUFFER_SIZE,  \
			USB_AUDIO_CLASS_DT_GET_TX_BUFFER_SIZE,  \
			_POINTER_TO_USB_PDEV ,           \
			_USB_AUDIO_CLASS_CALLBACK_PDEV,  \
			_POINTER_TO_USB_DESCRIPTOR_PDEV, \
			USB_AUDIO_CLASS_DT_HOST_OUT_SAMPLE_RATE_HZ, \
			USB_AUDIO_CLASS_DT_HOST_IN_SAMPLE_RATE_HZ,  \
			USB_AUDIO_CLASS_DT_NUM_OF_RX_BYTES_PER_SAMPLE, \
			USB_AUDIO_CLASS_DT_NUM_OF_TX_BYTES_PER_SAMPLE, \
			USB_AUDIO_CLASS_DT_NUM_OF_RX_CHANNELS,  \
			USB_AUDIO_CLASS_DT_NUM_OF_TX_CHANNELS,  \
			USB_AUDIO_CLASS_DT_NUM_OF_RX_BUFFERS,   \
			USB_AUDIO_CLASS_DT_NUM_OF_RX_OVER_UNDER_FLOW_BUFFERS_THR, \
			USB_AUDIO_CLASS_DT_PLAYBACK_TYPE,   \
			USB_AUDIO_CLASS_DT_ENABLE_RECORDING \
		}

	#endif

#include "add_component.h"

/*
 *  device specific defines should be undefined
 *  after calling #include "add_static_dev.h"
 */
#undef USB_AUDIO_CLASS_DT_USB_PDEV
#undef _POINTER_TO_USB_PDEV
#undef _USB_AUDIO_CLASS_CALLBACK_PDEV
#undef USB_AUDIO_CLASS_DT_CALLBACK_PDEV
#undef USB_AUDIO_CLASS_DT_BUFFER_SIZE
#undef USB_AUDIO_CLASS_DT_NUM_OF_BYTES_PER_SAMPLE
#undef USB_AUDIO_CLASS_DT_USB_DESCRIPTOR_PDEV
#undef _POINTER_TO_USB_DESCRIPTOR_PDEV
#undef USB_AUDIO_CLASS_DT_NUM_OF_RX_CHANNELS
#undef USB_AUDIO_CLASS_DT_NUM_OF_TX_CHANNELS
#undef USB_AUDIO_CLASS_DT_NUM_OF_RX_BUFFERS
#undef USB_AUDIO_CLASS_DT_NUM_OF_RX_OVER_UNDER_FLOW_BUFFERS_THR
#undef USB_AUDIO_CLASS_DT_HOST_OUT_SAMPLE_RATE_HZ
#undef USB_AUDIO_CLASS_DT_HOST_IN_SAMPLE_RATE_HZ
#undef USB_AUDIO_CLASS_DT_GET_RX_BUFFER_SIZE
#undef USB_AUDIO_CLASS_DT_GET_TX_BUFFER_SIZE
#undef USB_AUDIO_CLASS_DT_NUM_OF_RX_BYTES_PER_SAMPLE
#undef USB_AUDIO_CLASS_DT_NUM_OF_TX_BYTES_PER_SAMPLE
