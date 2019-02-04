/******************************************************************************
 * @file     usb_audio_class.c
 * @brief
 *
 * @note
 *
 ******************************************************************************/

/*!<Includes */
#include "_project_typedefs.h"
#include "_project_defines.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "dev_management_api.h"

#include "usb_device_api.h"
#include "usb_audio_class_api.h"
#include "usb_audio_class.h"
#include "usb_device_descriptors_api.h"

/*following line add module to available module list for dynamic device tree*/
#include "usb_audio_class_add_component.h"


#if !defined(BYTES_PER_PCM_CHANNEL)
	#error "BYTES_PER_PCM_CHANNEL should be defined in project header files"
#endif
#if !defined(NUM_OF_CHANNELS)
	#error "NUM_OF_CHANNELS should be defined in project header files"
#endif
#if !defined(SAMPLE_RATE)
	#error "SAMPLE_RATE  should be defined"
#endif

#define REC_TERMINAL_ID    0x04
#define PLAY_TERMINAL_ID   0x01

typedef enum
{
	USB_AUDIO_CLASS_BUFF_IDLE,
	USB_AUDIO_CLASS_BUFF_IN_PROCCESS,
	USB_AUDIO_CLASS_BUFF_TX_DATA_IS_FILLING,
	USB_AUDIO_CLASS_BUFF_TX_DATA_IS_READY,
	USB_AUDIO_CLASS_BUFF_RX_RADA_READY,
	USB_AUDIO_CLASS_BUFF_RX_RADA_PROCESSING
} USB_AUDIO_CLASS_buff_state_t;


/***************************************************/

#if !defined(SAMPLE_RATE)
	#error "SAMPLE_RATE  should be defined"
#endif

extern void uac_class_request(
		struct dev_desc_t *callback_dev, uint8_t *request);
extern void new_usb_audio_received(
		struct dev_desc_t *adev, uint8_t const *buff, size_t size);
extern void new_usb_audio_requested(struct dev_desc_t *adev);

#define USE_OUTPUT_ENDPOINT

#define TO_BE_SET_AUTOMATICALLY   0

#define B3VAL(x) (x&0xFF), ((x >>8) & 0xFF), ((x >>16) & 0xFF)
#define REC_RATE         SAMPLE_RATE
#define ALT2_PLAY_RATE   SAMPLE_RATE
#define AUTO_SET_PCKT_SIZE_MSB   0x00
#define AUTO_SET_PCKT_SIZE_LSB   0x00

#define AUDIO_CLASS             0x01
#define AUDIO_CTL_SUBCLASS      0x01
#define AUDIO_STREAM_SUBCLASS   0x02

static uint8_t const interface_association_descriptor[]=
{
	/*  Interface Association Descriptor:  */
	0x08,
	0x0B ,  //    bDescriptorType
	TO_BE_SET_AUTOMATICALLY, // bFirstInterface
#ifdef USE_OUTPUT_ENDPOINT
	0x03    , // bInterfaceCount
#else
	0x02    , // bInterfaceCount
#endif
	/* bFunctionClass,
	 * Microsoft recommend to match with first bInterfaceClass
	 */
	AUDIO_CLASS,

	/* bFunctionSubClass,
	 * Microsoft recommend to match with first bInterfaceSubClass
	 */
	AUDIO_CTL_SUBCLASS,

	0x00    , // bFunctionProtocol
	0x02    ,//  iFunction
};

static uint8_t const audio_control_interface[] =
{
// Standard Audio Control Interface Descriptor
	0x09,        // bLength
	0x04,        // bDescriptorType (Interface)
	TO_BE_SET_AUTOMATICALLY,        // bInterfaceNumber 0
	0x00,        // bAlternateSetting
	0x00,        // bNumEndpoints 0
	AUDIO_CLASS,        // bInterfaceClass (Audio)
	AUDIO_CTL_SUBCLASS,        // bInterfaceSubClass (Audio Control)
	0x00,        // bInterfaceProtocol - must be 0
	0x00,        // iInterface (String Index) - Null string

// Class-Specific Audio Control Interface Descriptor
#ifdef USE_OUTPUT_ENDPOINT
	0x0A,        // bLength
#else
	0x09,        // bLength
#endif
	0x24,        // bDescriptorType (See Next Line)
	0x01,        // bDescriptorSubtype (CS_INTERFACE -> HEADER)
	0x00, 0x01,  // bcdADC 1.00
#ifdef USE_OUTPUT_ENDPOINT
	0x48, 0x00,  // wTotalLength
	0x02,        // binCollection 0x02 (number of streaming interfaces)
#else
	0x28, 0x00,  // wTotalLength
	0x01,        // binCollection 0x01 (number of streaming interfaces)
#endif
	TO_BE_SET_AUTOMATICALLY,        // baInterfaceNr 1
#ifdef USE_OUTPUT_ENDPOINT
	TO_BE_SET_AUTOMATICALLY,        // baInterfaceNr 2

// Input Terminal Descriptor (Speakers)
	0x0C,        // bLength
	0x24,        // bDescriptorType (See Next Line)
	0x02,        // bDescriptorSubtype (CS_INTERFACE -> INPUT_TERMINAL)
	PLAY_TERMINAL_ID,   // bTerminalID
	0x01, 0x01,  // wTerminalType (USB Streaming)
	0x00,        // bAssocTerminal
	0x02,        // bNrChannels 2
	0x03, 0x00,  // wChannelConfig (Left and Right Front)
	0x00,        // iChannelNames
	0x00,        // iTerminal

// Feature Unit Descriptor
	0x0A,        // bLength
	0x24,        // bDescriptorType (See Next Line)
	0x06,        // bDescriptorSubtype (CS_INTERFACE -> FEATURE_UNIT)
	PLAY_FEATURE_UNITID, // bUnitID, used in all requests to address this Unit
	PLAY_TERMINAL_ID,    // bSourceID
	0x01,        // bControlSize 1
	0x01, 0x02,  // bmaControls[0] (Mute)
	0x02, 0x00,  // bmaControls[1] (Volume)

// Output Terminal Descriptor
	0x09,        // bLength
	0x24,        // bDescriptorType (See Next Line)
	0x03,        // bDescriptorSubtype (CS_INTERFACE -> OUTPUT_TERMINAL)
	0x03,        // bTerminalID
	0x01, 0x03,  // wTerminalType (Speaker)
	0x00,        // bAssocTerminal
	PLAY_FEATURE_UNITID,        // bSourceID
	0x00,        // iTerminal
#endif
// Input Terminal Descriptor (Microphone)
	0x0C,        // bLength
	0x24,        // bDescriptorType (See Next Line)
	0x02,        // bDescriptorSubtype (CS_INTERFACE -> INPUT_TERMINAL)
	REC_TERMINAL_ID,   // bTerminalID
	0x01, 0x02,  // wTerminalType (Microphone)
	0x00,        // bAssocTerminal
	0x02,        // bNrChannels 2
	0x03, 0x00,  // wChannelConfig (Left and Right Front)
	0x00,        // iChannelNames
	0x00,        // iTerminal

// Feature Unit Descriptor
	0x0A,        // bLength
	0x24,        // bDescriptorType (See Next Line)
	0x06,        // bDescriptorSubtype (CS_INTERFACE -> FEATURE_UNIT)
	REC_FEATURE_UNITID,// bUnitID, used in all requests to address this Unit
	REC_TERMINAL_ID,   // bSourceID
	0x01,        // bControlSize 1
    0x01,        // bmaControls(0) (master control)
    0x02,        // bmaControls(1)
    0x02,        // bmaControls(2)
    0x00,        // iFeature

// Output Terminal Descriptor for USB Streaming
	0x09,        // bLength
	0x24,        // bDescriptorType (See Next Line)
	0x03,        // bDescriptorSubtype (CS_INTERFACE -> OUTPUT_TERMINAL)
	0x02,        // bTerminalID
	0x01, 0x01,  // wTerminalType (USB Streaming)
	0x00,        // bAssocTerminal
	REC_FEATURE_UNITID,        // bSourceID
	0x00,        // iTerminal
};

static uint8_t const in_interface[] = {
	0x09,        // bLength
	0x04,        // bDescriptorType (Interface)
	TO_BE_SET_AUTOMATICALLY,        // bInterfaceNumber
	0x00,        // bAlternateSetting
	0x00,        // bNumEndpoints 0
	AUDIO_CLASS,        // bInterfaceClass (Audio)
	AUDIO_STREAM_SUBCLASS,        // bInterfaceSubClass (Audio Streaming)
	0x00,        // bInterfaceProtocol
	0x00        // iInterface (String Index)
};

static uint8_t const in_alt_interface[] = {
	0x09,        // bLength
	0x04,        // bDescriptorType (Interface)
	TO_BE_SET_AUTOMATICALLY,        // bInterfaceNumber
	0x01,        // bAlternateSetting
	0x01,        // bNumEndpoints 1
	AUDIO_CLASS,        // bInterfaceClass (Audio)
	AUDIO_STREAM_SUBCLASS,        // bInterfaceSubClass (Audio Streaming)
	0x00,        // bInterfaceProtocol
	0x00,        // iInterface (String Index)

	0x07,        // bLength
	0x24,        // bDescriptorType (See Next Line)
	0x01,        // bDescriptorSubtype (CS_INTERFACE -> AS_GENERAL)
	0x02,        // bTerminalLink
	0x01,        // bDelay 1
	0x01, 0x00,  // wFormatTag (PCM)

	0x0B,        // bLength
	0x24,        // bDescriptorType (See Next Line)
	0x02,        // bDescriptorSubtype (CS_INTERFACE -> FORMAT_TYPE)
	0x01,        // bFormatType 1
	0x02,        // bNrChannels (Stereo)
#if (2 == BYTES_PER_PCM_CHANNEL)
	0x02,        // bSubFrameSize 2
	0x10,        // bBitResolution 16
#elif (4 == BYTES_PER_PCM_CHANNEL)
	0x04,        // bSubFrameSize 4
	0x20,        // bBitResolution 32
#endif
	0x01,        // bSamFreqType 1
	B3VAL(REC_RATE),

	0x09,        // bLength
	0x05,        // bDescriptorType (See Next Line)
	TO_BE_SET_AUTOMATICALLY,        // bEndpointAddress (IN/D2H)
	0x0D,        // bmAttributes (Isochronous, Sync, Data EP)
	AUTO_SET_PCKT_SIZE_LSB, AUTO_SET_PCKT_SIZE_MSB,  // wMaxPacketSize
	0x01,        // bInterval 1 (unit depends on device speed)
	0x00,        // bRefresh
	0x00,        // bSyncAddress

	0x07,        // bLength
	0x25,        // bDescriptorType (See Next Line)
	0x01,        // bDescriptorSubtype (CS_ENDPOINT -> EP_GENERAL)
	0x00,        // bmAttributes (sample rate control ? )
	0x00,        // bLockDelayUnits
	0x00, 0x00  // wLockDelay 0
};

static uint8_t const out_interface[] = {
	0x09,        // bLength
	0x04,        // bDescriptorType (Interface)
	TO_BE_SET_AUTOMATICALLY,        // bInterfaceNumber
	0x00,        // bAlternateSetting
	0x00,        // bNumEndpoints 0
	AUDIO_CLASS,        // bInterfaceClass (Audio)
	AUDIO_STREAM_SUBCLASS,        // bInterfaceSubClass (Audio Streaming)
	0x00,        // bInterfaceProtocol
	0x00        // iInterface (String Index)
};

static uint8_t const out_alt_interface[] = {
	0x09,        // bLength
	0x04,        // bDescriptorType (Interface)
	TO_BE_SET_AUTOMATICALLY,        // bInterfaceNumber
	0x01,        // bAlternateSetting
	0x01,        // bNumEndpoints 1
	AUDIO_CLASS,        // bInterfaceClass (Audio)
	AUDIO_STREAM_SUBCLASS,        // bInterfaceSubClass (Audio Streaming)
	0x00,        // bInterfaceProtocol
	0x00,        // iInterface (String Index)

	0x07,        // bLength
	0x24,        // bDescriptorType (See Next Line)
	0x01,        // bDescriptorSubtype (CS_INTERFACE -> AS_GENERAL)
	0x01,        // bTerminalLink
	0x01,        // bDelay 1
	0x01, 0x00,  // wFormatTag (PCM)

	0x0B,        // bLength
	0x24,        // bDescriptorType (See Next Line)
	0x02,        // bDescriptorSubtype (CS_INTERFACE -> FORMAT_TYPE)
	0x01,        // bFormatType 1
	0x02,        // bNrChannels (Stereo)
#if (2 == BYTES_PER_PCM_CHANNEL)
	0x02,        // bSubFrameSize 2
	0x10,        // bBitResolution 16
#elif (4 == BYTES_PER_PCM_CHANNEL)
	0x04,        // bSubFrameSize 4
	0x20,        // bBitResolution 32
#endif
	0x01,        // bSamFreqType 1
	B3VAL(ALT2_PLAY_RATE),
	//B3VAL(PLAY_RATE),
	//B3VAL(ALT1_PLAY_RATE),


	0x09,        // bLength
	0x05,        // bDescriptorType (See Next Line)
	TO_BE_SET_AUTOMATICALLY,        // bEndpointAddress (OUT/H2D)
	0x0D,        // bmAttributes (Isochronous, Sync, Data EP)
	AUTO_SET_PCKT_SIZE_LSB, AUTO_SET_PCKT_SIZE_MSB,  // wMaxPacketSize
	0x01,        // bInterval 1 (unit depends on device speed)
	0x00,        // bRefresh
	0x00,        // bSyncAddress

	0x07,        // bLength
	0x25,        // bDescriptorType (See Next Line)
	0x01,        // bDescriptorSubtype (CS_ENDPOINT -> EP_GENERAL)
	0x80,        // bmAttributes (none)
	0x00,        // bLockDelayUnits
	0x00, 0x00  // wLockDelay 0
};



static void configure_endpoints(struct dev_desc_t *adev,
		struct usb_audio_class_cfg_t *cfg_hndl,
		uint8_t *i_in_alt1, uint8_t *i_out_alt1, uint16_t audio_pckt_size)
{
	struct usb_audio_class_runtime_t *runtime_hndl;
	struct dev_desc_t *usb_hw;
	struct set_endpoints_t set_endpoints;
	usb_dev_in_endpoint_callback_func_t   in_func_arr[2];
	uint8_t   endpoints_num_arr[2];
	usb_dev_out_endpoint_callback_func_t   out_func_arr[2];
	uint8_t    endpoints_type_arr[2];
	uint16_t   max_pckt_sizes[2];
	uint8_t in_endpoint_num;
	uint8_t out_endpoint_num;
	uint16_t endpoint_packet_size;

	runtime_hndl = DEV_GET_RUNTIME_DATA_POINTER(adev);

	endpoint_packet_size = audio_pckt_size;
	usb_hw = cfg_hndl->usb_hw;

#ifdef USE_OUTPUT_ENDPOINT
	set_endpoints.num_of_endpoints = 2;
#else
	set_endpoints.num_of_endpoints = 1;
#endif
	set_endpoints.endpoints_num_arr = endpoints_num_arr;
	out_func_arr[0] = NULL;
	out_func_arr[1] = new_usb_audio_received;
	in_func_arr[0] = new_usb_audio_requested;
	in_func_arr[1] = NULL;
	set_endpoints.out_func_arr = out_func_arr;
	set_endpoints.in_func_arr = in_func_arr;
	set_endpoints.callback_dev = adev;
	max_pckt_sizes[0] = endpoint_packet_size;
	max_pckt_sizes[1] = endpoint_packet_size;
	set_endpoints.max_pckt_sizes = max_pckt_sizes;
	endpoints_type_arr[0] = USB_DEVICE_API_EP_TYPE_ISO_IN;
	endpoints_type_arr[1] = USB_DEVICE_API_EP_TYPE_ISO_OUT;
	set_endpoints.endpoints_type_arr = endpoints_type_arr;
	DEV_IOCTL_1_PARAMS(usb_hw, IOCTL_USB_DEVICE_SET_ENDPOINTS, &set_endpoints);

	in_endpoint_num = endpoints_num_arr[0];
	out_endpoint_num = endpoints_num_arr[1];
	i_in_alt1[27 + 2] = 0x80 | in_endpoint_num;// 0x80 for IN endpoint
	runtime_hndl->in_endpoint_num = in_endpoint_num;
	i_in_alt1[27 + 4] = endpoint_packet_size & 0xFF;
	i_in_alt1[27 + 5] = (endpoint_packet_size >> 8) & 0xFF;
	i_out_alt1[27 + 2] = out_endpoint_num;
	i_out_alt1[27 + 4] = endpoint_packet_size & 0xFF;
	i_out_alt1[27 + 5] = (endpoint_packet_size >> 8) & 0xFF;
}


static void update_configuration_desc(struct dev_desc_t *adev,
		struct usb_audio_class_cfg_t *cfg_hndl,
	struct usb_descriptors_alloc_interfaces_t *usb_descriptors_alloc_interfaces,
	uint16_t audio_pckt_size)
{
	struct dev_desc_t *usb_descriptors_dev;
	struct usb_descriptors_add_interface_t usb_desc_add_interface;
	uint8_t *iad;
	uint8_t *i_ctl;
	uint8_t *i_in_alt0;
	uint8_t *i_in_alt1;
	uint8_t *i_out_alt0;
	uint8_t *i_out_alt1;

	usb_descriptors_dev = cfg_hndl->usb_descriptors_dev;

	iad = (uint8_t*)malloc(sizeof(interface_association_descriptor)) ;
	memcpy(iad, interface_association_descriptor,
						sizeof(interface_association_descriptor));
	iad[2] = usb_descriptors_alloc_interfaces->interfaces_num[0];
	DEV_IOCTL_1_PARAMS(usb_descriptors_dev,
			USB_DEVICE_DESCRIPTORS_ADD_INTERFACE_ASSOCIATION_DESCRIPTOR, iad);
	free(iad);


	i_ctl = (uint8_t*)malloc(sizeof(audio_control_interface)) ;
	memcpy(i_ctl, audio_control_interface, sizeof(audio_control_interface));
	i_ctl[2] = usb_descriptors_alloc_interfaces->interfaces_num[0];
	i_ctl[17] = usb_descriptors_alloc_interfaces->interfaces_num[1];
#ifdef USE_OUTPUT_ENDPOINT
	i_ctl[18] = usb_descriptors_alloc_interfaces->interfaces_num[2];
#endif

	usb_desc_add_interface.interface_desc = i_ctl;
	usb_desc_add_interface.interface_desc_size =
								sizeof(audio_control_interface);
	usb_desc_add_interface.alt_interface_desc = NULL;
	DEV_IOCTL_1_PARAMS(usb_descriptors_dev,
			USB_DEVICE_DESCRIPTORS_ADD_INTERFACE, &usb_desc_add_interface);
	free(i_ctl);


	i_in_alt0 = (uint8_t*)malloc(sizeof(in_interface)) ;
	memcpy(i_in_alt0, in_interface, sizeof(in_interface));
	i_in_alt1 = (uint8_t*)malloc(sizeof(in_alt_interface)) ;
	memcpy(i_in_alt1, in_alt_interface, sizeof(in_alt_interface));
	i_out_alt0 = (uint8_t*)malloc(sizeof(out_interface)) ;
	memcpy(i_out_alt0, out_interface, sizeof(out_interface));
	i_out_alt1 = (uint8_t*)malloc(sizeof(out_alt_interface)) ;
	memcpy(i_out_alt1, out_alt_interface, sizeof(out_alt_interface));
	i_in_alt0[2] = usb_descriptors_alloc_interfaces->interfaces_num[1];
	i_in_alt1[2] = usb_descriptors_alloc_interfaces->interfaces_num[1];
	i_out_alt0[2] = usb_descriptors_alloc_interfaces->interfaces_num[2];
	i_out_alt1[2] = usb_descriptors_alloc_interfaces->interfaces_num[2];

	configure_endpoints(adev, cfg_hndl, i_in_alt1, i_out_alt1, audio_pckt_size);

	usb_desc_add_interface.interface_desc = i_in_alt0;
	usb_desc_add_interface.interface_desc_size = sizeof(in_interface);
	usb_desc_add_interface.alt_interface_desc = i_in_alt1;
	usb_desc_add_interface.alt_interface_desc_size = sizeof(in_alt_interface);
	usb_desc_add_interface.is_hid_interface = 0;
	DEV_IOCTL_1_PARAMS(usb_descriptors_dev,
			USB_DEVICE_DESCRIPTORS_ADD_INTERFACE, &usb_desc_add_interface);

#ifdef USE_OUTPUT_ENDPOINT
	usb_desc_add_interface.interface_desc = i_out_alt0;
	usb_desc_add_interface.interface_desc_size = sizeof(out_interface);
	usb_desc_add_interface.alt_interface_desc = i_out_alt1;
	usb_desc_add_interface.alt_interface_desc_size = sizeof(out_alt_interface);
	usb_desc_add_interface.is_hid_interface = 0;
	DEV_IOCTL_1_PARAMS(usb_descriptors_dev,
			USB_DEVICE_DESCRIPTORS_ADD_INTERFACE, &usb_desc_add_interface);
#endif
	free(i_in_alt0);
	free(i_in_alt1);
	free(i_out_alt0);
	free(i_out_alt1);
}


void add_audio_class_device(struct dev_desc_t *adev,
		struct usb_audio_class_cfg_t *cfg_hndl,
		struct usb_audio_class_runtime_t *runtime_hndl,
		uint16_t audio_pckt_size)
{
	struct register_interfaces_t register_interfaces;
	struct register_interface_t register_interface[3];
	struct usb_descriptors_alloc_interfaces_t usb_descriptors_alloc_interfaces;
	struct dev_desc_t *usb_descriptors_dev;
	struct dev_desc_t *usb_hw;

#ifdef USE_OUTPUT_ENDPOINT
	usb_descriptors_alloc_interfaces.num_of_interfaces = 3;
#else
	usb_descriptors_alloc_interfaces.num_of_interfaces = 2;
#endif
	usb_descriptors_dev = cfg_hndl->usb_descriptors_dev;
	DEV_IOCTL_1_PARAMS(usb_descriptors_dev,
			USB_DEVICE_DESCRIPTORS_ALLOC_INTERFACES_NUMBERS,
			&usb_descriptors_alloc_interfaces);

	update_configuration_desc(
			adev, cfg_hndl, &usb_descriptors_alloc_interfaces, audio_pckt_size);

	register_interface[0].interfaces_num =
			usb_descriptors_alloc_interfaces.interfaces_num[0];
	register_interface[0].interface_func = uac_class_request;
	register_interface[1].interfaces_num =
			usb_descriptors_alloc_interfaces.interfaces_num[1];
	register_interface[1].interface_func = uac_class_request;
	register_interface[2].interfaces_num =
			usb_descriptors_alloc_interfaces.interfaces_num[2];
	register_interface[2].interface_func = uac_class_request;

#ifdef USE_OUTPUT_ENDPOINT
	register_interfaces.num_of_interfaces = 3;
#else
	register_interfaces.num_of_interfaces = 2;
#endif
	register_interfaces.register_interface_arr = register_interface;
	register_interfaces.callback_dev = adev;
	usb_hw = cfg_hndl->usb_hw;
	DEV_IOCTL_1_PARAMS(usb_hw,
			IOCTL_USB_DEVICE_REGISTER_INTERFACES, &register_interfaces);
}
