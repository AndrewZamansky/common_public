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
#include "errors_api.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "dev_management_api.h"

#include "usb_device_api.h"
#include "usb_audio_class_api.h"
#include "usb_audio_class.h"
#include "usb_device_descriptors_api.h"


#define REC_STREAM_TERMINAL_ID    0x02
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

extern void uac_interface_class_request(
		struct dev_desc_t *callback_dev, uint8_t *request);
extern void uac_endpoint_class_request(
		struct dev_desc_t *callback_dev, uint8_t *request);
extern void new_usb_audio_received(
		struct dev_desc_t *adev, uint8_t const *buff, size_t size);
extern void new_usb_audio_requested(struct dev_desc_t *adev);
extern void new_sample_rate_requested(struct dev_desc_t *adev);


#define WILL_BE_SET_ON_INIT   0

#define B2VAL(x) (x & 0xFF), ((x >> 8) & 0xFF)
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
	WILL_BE_SET_ON_INIT, // bFirstInterface
	WILL_BE_SET_ON_INIT , // bInterfaceCount

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

static uint8_t const audio_control_interface_start[] =
{
// Standard Audio Control Interface Descriptor
	0x09,        // bLength
	0x04,        // bDescriptorType (Interface)
	WILL_BE_SET_ON_INIT,        // bInterfaceNumber 0
	0x00,        // bAlternateSetting
	0x00,        // bNumEndpoints 0
	AUDIO_CLASS,        // bInterfaceClass (Audio)
	AUDIO_CTL_SUBCLASS,        // bInterfaceSubClass (Audio Control)
	0x00,        // bInterfaceProtocol - must be 0
	0x00,        // iInterface (String Index) - Null string

// Class-Specific Audio Control Interface Descriptor
	WILL_BE_SET_ON_INIT,        // bLength 0x8, 0x9 or 0xa
	0x24,        // bDescriptorType (See Next Line)
	0x01,        // bDescriptorSubtype (CS_INTERFACE -> HEADER)
	0x00, 0x01,  // bcdADC 1.00
	WILL_BE_SET_ON_INIT, WILL_BE_SET_ON_INIT,  // wTotalLength
	WILL_BE_SET_ON_INIT, // binCollection (number of streaming interfaces)
	WILL_BE_SET_ON_INIT,        // baInterfaceNr 1
	WILL_BE_SET_ON_INIT        // baInterfaceNr 2
};


static uint8_t const audio_out_terminal_descriptors[] =
{
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
	0x00         // iTerminal
};


static uint8_t const audio_in_terminal_descriptors[] =
{
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
	REC_STREAM_TERMINAL_ID,        // bTerminalID
	0x01, 0x01,  // wTerminalType (USB Streaming)
	0x00,        // bAssocTerminal
	REC_FEATURE_UNITID,        // bSourceID
	0x00,        // iTerminal
};

static uint8_t const in_interface[] = {
	0x09,        // bLength
	0x04,        // bDescriptorType (Interface)
	WILL_BE_SET_ON_INIT,        // bInterfaceNumber
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
	WILL_BE_SET_ON_INIT,        // bInterfaceNumber
	0x01,        // bAlternateSetting
	0x01,        // bNumEndpoints 1
	AUDIO_CLASS,        // bInterfaceClass (Audio)
	AUDIO_STREAM_SUBCLASS,        // bInterfaceSubClass (Audio Streaming)
	0x00,        // bInterfaceProtocol
	0x00,        // iInterface (String Index)

	0x07,        // bLength
	0x24,        // bDescriptorType (See Next Line)
	0x01,        // bDescriptorSubtype (CS_INTERFACE -> AS_GENERAL)
	REC_STREAM_TERMINAL_ID,        // bTerminalLink
	0x01,        // bDelay 1
	0x01, 0x00,  // wFormatTag (PCM)

	0x0B,        // bLength
	0x24,        // bDescriptorType (See Next Line)
	0x02,        // bDescriptorSubtype (CS_INTERFACE -> FORMAT_TYPE)
	0x01,        // bFormatType 1
	WILL_BE_SET_ON_INIT,        // bNrChannels (Stereo)
	WILL_BE_SET_ON_INIT,        // bSubFrameSize in bytes
	WILL_BE_SET_ON_INIT,        // bBitResolution in bits
	0x01,        // bSamFreqType 1
	WILL_BE_SET_ON_INIT, WILL_BE_SET_ON_INIT, WILL_BE_SET_ON_INIT,// sample rate

	0x09,        // bLength
	0x05,        // bDescriptorType (See Next Line)
	WILL_BE_SET_ON_INIT,        // bEndpointAddress (IN/D2H)
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
	WILL_BE_SET_ON_INIT,        // bInterfaceNumber
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
	WILL_BE_SET_ON_INIT,        // bInterfaceNumber
	0x01,        // bAlternateSetting
	WILL_BE_SET_ON_INIT,  // bNumEndpoints 1 for sync, 2 for async
	AUDIO_CLASS,        // bInterfaceClass (Audio)
	AUDIO_STREAM_SUBCLASS,        // bInterfaceSubClass (Audio Streaming)
	0x00,        // bInterfaceProtocol
	0x00,        // iInterface (String Index)

	0x07,        // bLength
	0x24,        // bDescriptorType (See Next Line)
	0x01,        // bDescriptorSubtype (CS_INTERFACE -> AS_GENERAL)
	PLAY_TERMINAL_ID,   // bTerminalLink
	0x01,        // bDelay 1
	0x01, 0x00,  // wFormatTag (PCM)

	0x0B,        // bLength
	0x24,        // bDescriptorType (See Next Line)
	0x02,        // bDescriptorSubtype (CS_INTERFACE -> FORMAT_TYPE)
	0x01,        // bFormatType 1
	WILL_BE_SET_ON_INIT,        // bNrChannels (Stereo)
	WILL_BE_SET_ON_INIT,        // bSubFrameSize in bytes
	WILL_BE_SET_ON_INIT,        // bBitResolution in bits
	0x01,        // bSamFreqType 1
	WILL_BE_SET_ON_INIT, WILL_BE_SET_ON_INIT, WILL_BE_SET_ON_INIT,// sample rate

	0x09,        // bLength
	0x05,        // bDescriptorType (See Next Line)
	WILL_BE_SET_ON_INIT,  // bEndpointAddress (OUT/H2D)
	WILL_BE_SET_ON_INIT,  // bmAttributes (Isochronous, Sync, Data EP)
	AUTO_SET_PCKT_SIZE_LSB, AUTO_SET_PCKT_SIZE_MSB,  // wMaxPacketSize
	0x01,        // bInterval 1 (unit depends on device speed)
	0x00,        // bRefresh
	WILL_BE_SET_ON_INIT,        // bSyncAddress

	0x07,        // bLength
	0x25,        // bDescriptorType (See Next Line)
	0x01,        // bDescriptorSubtype (CS_ENDPOINT -> EP_GENERAL)
	0x01,        // bmAttributes (none) was 0x80
	0x00,        // bLockDelayUnits
	0x00, 0x00,  // wLockDelay 0

	// feedback endpoint
	0x09,        // bLength
	0x05,        // bDescriptorType (See Next Line)
	WILL_BE_SET_ON_INIT,  // bEndpointAddress (OUT/H2D)
	0x01,  // bmAttributes (Isochronous, Sync = none)
	B2VAL(0x4),  // wMaxPacketSize
	0x01,        // bInterval 1 (unit depends on device speed)
	0x05,        // bRefresh
	0x00        // bSyncAddress
};


#define ISO_ENDPOINT  0x01
#define SYNCHRONOUS   0x0c
#define ASYNCHRONOUS  0x04

#define FORMAT_OFFSET                (0x9 + 0x7)
#define IN_ENDPOINT_OFFSET           (0x9 + 0x7 + 0xB)
#define OUT_ENDPOINT_OFFSET          (0x9 + 0x7 + 0xB)
#define IN_FEEDBACK_ENDPOINT_OFFSET  (0x9 + 0x7 + 0xB + 0x9 + 0x07)

static void configure_endpoints(struct dev_desc_t *adev,
		struct usb_audio_class_cfg_t *cfg_hndl,
		uint8_t *i_in_alt1, uint8_t *i_out_alt1)
{
	struct usb_audio_class_runtime_t *runtime_hndl;
	struct dev_desc_t *usb_hw;
	struct set_endpoints_t set_endpoints;
	usb_dev_in_endpoint_callback_func_t   in_func_arr[3];
	uint8_t   endpoints_num_arr[3];
	usb_dev_out_endpoint_callback_func_t   out_func_arr[3];
	usb_dev_endpoint_request_callback_func_t  endpoint_request_callback_func[3];
	uint8_t    endpoints_type_arr[3];
	uint16_t   max_pckt_sizes[3];
	uint8_t in_endpoint_num;
	uint8_t  playback_type;
	uint8_t  enable_recording;
	uint8_t  num_of_endpoints;
	uint8_t  curr_endpoint_indx;
	uint8_t  *endpoint_desc;
	uint16_t max_host_out_pckt_size;
	uint16_t max_host_in_pckt_size;

	runtime_hndl = DEV_GET_RUNTIME_DATA_POINTER(adev);

	usb_hw = cfg_hndl->usb_hw;
	playback_type = cfg_hndl->playback_type;
	enable_recording = cfg_hndl->enable_recording;

	// bytes per frame, so per ms,  so /1000   .
	// + 1  for fractional sample rate like 44.1khz
	max_host_out_pckt_size = ((cfg_hndl->host_out_sample_rate_hz / 1000) +
			1 + HOST_OUT_MAX_ADDITIONAL_SAMPLES) *
			( cfg_hndl->num_of_rx_bytes_per_sample * cfg_hndl->num_rx_channels);
	max_host_in_pckt_size = runtime_hndl->normal_host_in_pckt_size +
		(cfg_hndl->num_of_tx_bytes_per_sample * cfg_hndl->num_tx_channels);

	num_of_endpoints = 0;
	if (0 != enable_recording)
	{
		out_func_arr[num_of_endpoints] = NULL;
		in_func_arr[num_of_endpoints] = new_usb_audio_requested;
		endpoint_request_callback_func[num_of_endpoints] =
											uac_endpoint_class_request;
		max_pckt_sizes[num_of_endpoints] = max_host_in_pckt_size;
		endpoints_type_arr[num_of_endpoints] = USB_DEVICE_API_EP_TYPE_ISO_IN;
		num_of_endpoints++;
	}

	if (USB_AUDIO_CLASS_NO_PLAYBACK != playback_type)
	{
		out_func_arr[num_of_endpoints] = new_usb_audio_received;
		in_func_arr[num_of_endpoints] = NULL;
		endpoint_request_callback_func[num_of_endpoints] =
											uac_endpoint_class_request;
		max_pckt_sizes[num_of_endpoints] = max_host_out_pckt_size;
		endpoints_type_arr[num_of_endpoints] = USB_DEVICE_API_EP_TYPE_ISO_OUT;
		num_of_endpoints++;
	}
	if (USB_AUDIO_CLASS_ASYNC_PLAYBACK == playback_type)
	{
		out_func_arr[num_of_endpoints] = NULL;
		in_func_arr[num_of_endpoints] = new_sample_rate_requested;
		endpoint_request_callback_func[num_of_endpoints] =
										uac_endpoint_class_request;
		max_pckt_sizes[num_of_endpoints] = 4;
		endpoints_type_arr[num_of_endpoints] = USB_DEVICE_API_EP_TYPE_ISO_IN;
		num_of_endpoints++;
	}

	set_endpoints.num_of_endpoints = num_of_endpoints;
	set_endpoints.endpoints_num_arr = endpoints_num_arr;
	set_endpoints.out_func_arr = out_func_arr;
	set_endpoints.in_func_arr = in_func_arr;
	set_endpoints.endpoint_request_callback_func =
									endpoint_request_callback_func;
	set_endpoints.callback_dev = adev;
	set_endpoints.max_pckt_sizes = max_pckt_sizes;
	set_endpoints.endpoints_type_arr = endpoints_type_arr;
	DEV_IOCTL_1_PARAMS(usb_hw, IOCTL_USB_DEVICE_SET_ENDPOINTS, &set_endpoints);

	curr_endpoint_indx = 0;
	if (0 != enable_recording)
	{
		in_endpoint_num = endpoints_num_arr[curr_endpoint_indx];
		endpoint_desc = &i_in_alt1[IN_ENDPOINT_OFFSET];
		endpoint_desc[2] = 0x80 | in_endpoint_num;// 0x80 for IN endpoint
		endpoint_desc[4] = max_host_in_pckt_size & 0xFF;
		endpoint_desc[5] = (max_host_in_pckt_size >> 8) & 0xFF;
		runtime_hndl->in_endpoint_num = in_endpoint_num;
		curr_endpoint_indx++;
	}

	if (USB_AUDIO_CLASS_NO_PLAYBACK != playback_type)
	{
		endpoint_desc = &i_out_alt1[OUT_ENDPOINT_OFFSET];
		endpoint_desc[2] = endpoints_num_arr[curr_endpoint_indx];
		endpoint_desc[4] = max_host_out_pckt_size & 0xFF;
		endpoint_desc[5] = (max_host_out_pckt_size >> 8) & 0xFF;
		runtime_hndl->out_endpoint_num = endpoints_num_arr[curr_endpoint_indx];
		curr_endpoint_indx++;

		if (USB_AUDIO_CLASS_SYNC_WITH_CORRECTIONS_PLAYBACK == playback_type)
		{
			i_out_alt1[4] = 0x01;
			endpoint_desc[3] = SYNCHRONOUS | ISO_ENDPOINT;
		}
		else if (USB_AUDIO_CLASS_ASYNC_PLAYBACK == playback_type)
		{
			uint8_t  in_feedback_endpoint_num;
			i_out_alt1[4] = 0x02;
			in_feedback_endpoint_num = endpoints_num_arr[curr_endpoint_indx];
			endpoint_desc[3] = ASYNCHRONOUS | ISO_ENDPOINT;
			endpoint_desc[8] = in_feedback_endpoint_num; // 0x80 for IN
			runtime_hndl->in_feedback_endpoint_num = in_feedback_endpoint_num;
			i_out_alt1[IN_FEEDBACK_ENDPOINT_OFFSET + 2] =
								0x80 | in_feedback_endpoint_num; // 0x80 for IN
		}
	}
}


static void fill_association_descriptor(struct usb_audio_class_cfg_t *cfg_hndl,
	struct usb_descriptors_alloc_interfaces_t *usb_descriptors_alloc_interfaces)
{
	uint8_t *iad;
	uint8_t num_of_interfaces;

	iad = (uint8_t*)malloc(sizeof(interface_association_descriptor)) ;
	errors_api_check_if_malloc_succeed(iad);
	memcpy(iad, interface_association_descriptor,
						sizeof(interface_association_descriptor));
	iad[2] = usb_descriptors_alloc_interfaces->interfaces_num[0];
	num_of_interfaces = 1;// +1 for control descriptor
	if (0 != cfg_hndl->enable_recording)
	{
		num_of_interfaces++;
	}
	if (USB_AUDIO_CLASS_NO_PLAYBACK != cfg_hndl->playback_type)
	{
		num_of_interfaces++;
	}
	iad[3] = num_of_interfaces;
	DEV_IOCTL_1_PARAMS(cfg_hndl->usb_descriptors_dev,
			USB_DEVICE_DESCRIPTORS_ADD_INTERFACE_ASSOCIATION_DESCRIPTOR, iad);
	free(iad);
}


static void fill_control_descriptor(struct usb_audio_class_cfg_t *cfg_hndl,
	struct usb_descriptors_alloc_interfaces_t *usb_descriptors_alloc_interfaces)
{
	uint8_t *i_ctl;
	uint8_t  playback_type;
	uint8_t  enable_recording;
	uint8_t  curr_interface_idx;
	uint8_t  interface_num_offset;
	uint8_t  control_interface_CS_size;// class specific descriptor size
	uint16_t  control_interface_size;
	uint16_t  control_interface_start_size;
	uint16_t termianl_descriptors_size;
	struct usb_descriptors_add_interface_t usb_desc_add_interface;

	playback_type = cfg_hndl->playback_type;
	enable_recording = cfg_hndl->enable_recording;

	termianl_descriptors_size = 0;
	control_interface_CS_size = 8;
	if (0 != enable_recording)
	{
		control_interface_CS_size++;
		termianl_descriptors_size += sizeof(audio_in_terminal_descriptors);
	}
	if (USB_AUDIO_CLASS_NO_PLAYBACK != playback_type)
	{
		control_interface_CS_size++;
		termianl_descriptors_size += sizeof(audio_out_terminal_descriptors);
	}
	control_interface_start_size = 9 + control_interface_CS_size;
	control_interface_size =
			control_interface_start_size + termianl_descriptors_size;
	i_ctl = (uint8_t*)malloc(control_interface_size);
	errors_api_check_if_malloc_succeed(i_ctl);

	memcpy(i_ctl, audio_control_interface_start, control_interface_start_size);
	i_ctl[2] = usb_descriptors_alloc_interfaces->interfaces_num[0];

	i_ctl[9] = control_interface_CS_size;  // size of descriptor
	i_ctl[14] = (termianl_descriptors_size + control_interface_CS_size) & 0xFF;
	i_ctl[15] =
		((termianl_descriptors_size + control_interface_CS_size) >> 8) & 0xFF;
	curr_interface_idx = 1;
	interface_num_offset = 0;
	if (0 != enable_recording)
	{
		i_ctl[9 + 8 + interface_num_offset] =
			usb_descriptors_alloc_interfaces->interfaces_num[
															curr_interface_idx];
		memcpy(&i_ctl[control_interface_start_size],
			audio_in_terminal_descriptors,
			sizeof(audio_in_terminal_descriptors));
		control_interface_start_size += sizeof(audio_in_terminal_descriptors);
		curr_interface_idx++;
		interface_num_offset++;
	}
	if (USB_AUDIO_CLASS_NO_PLAYBACK != playback_type)
	{
		i_ctl[9 + 8 + interface_num_offset]
			  = usb_descriptors_alloc_interfaces->interfaces_num[
															curr_interface_idx];
		memcpy(&i_ctl[control_interface_start_size],
			audio_out_terminal_descriptors,
			sizeof(audio_out_terminal_descriptors));
		curr_interface_idx++;
	}
	i_ctl[16] = curr_interface_idx - 1;   // number of all streaming interfaces
	usb_desc_add_interface.interface_desc = i_ctl;
	usb_desc_add_interface.interface_desc_size = control_interface_size;
	usb_desc_add_interface.alt_interface_desc = NULL;
	DEV_IOCTL_1_PARAMS(cfg_hndl->usb_descriptors_dev,
			USB_DEVICE_DESCRIPTORS_ADD_INTERFACE, &usb_desc_add_interface);
	free(i_ctl);
}


static void update_interfaces_desc(struct dev_desc_t *adev,
		struct usb_audio_class_cfg_t *cfg_hndl,
	struct usb_descriptors_alloc_interfaces_t *usb_descriptors_alloc_interfaces)
{
	struct dev_desc_t *usb_descriptors_dev;
	struct usb_descriptors_add_interface_t usb_desc_add_interface;
	uint8_t curr_interface_idx;
	uint8_t *i_in_alt0;
	uint8_t *i_in_alt1;
	uint8_t *i_out_alt0;
	uint8_t *i_out_alt1;
	uint8_t  playback_type;
	uint8_t  enable_recording;
	uint8_t num_of_bytes_per_sample;
	uint32_t host_out_sample_rate_hz;
	uint32_t host_in_sample_rate_hz;

	playback_type = cfg_hndl->playback_type;
	enable_recording = cfg_hndl->enable_recording;
	usb_descriptors_dev = cfg_hndl->usb_descriptors_dev;

	fill_association_descriptor(cfg_hndl, usb_descriptors_alloc_interfaces);

	fill_control_descriptor(cfg_hndl, usb_descriptors_alloc_interfaces);


	curr_interface_idx = 1;

	i_in_alt0 = NULL;
	i_in_alt1 = NULL;
	if (0 != enable_recording)
	{
		i_in_alt0 = (uint8_t*)malloc(sizeof(in_interface)) ;
		errors_api_check_if_malloc_succeed(i_in_alt0);
		memcpy(i_in_alt0, in_interface, sizeof(in_interface));
		i_in_alt1 = (uint8_t*)malloc(sizeof(in_alt_interface)) ;
		errors_api_check_if_malloc_succeed(i_in_alt1);
		memcpy(i_in_alt1, in_alt_interface, sizeof(in_alt_interface));
		i_in_alt0[2] = usb_descriptors_alloc_interfaces->interfaces_num[
															curr_interface_idx];
		i_in_alt1[2] = usb_descriptors_alloc_interfaces->interfaces_num[
															curr_interface_idx];
		curr_interface_idx++;
	}

	i_out_alt0 = NULL;
	i_out_alt1 = NULL;
	if (USB_AUDIO_CLASS_NO_PLAYBACK != playback_type)
	{
		i_out_alt0 = (uint8_t*)malloc(sizeof(out_interface)) ;
		errors_api_check_if_malloc_succeed(i_out_alt0);
		memcpy(i_out_alt0, out_interface, sizeof(out_interface));
		i_out_alt1 = (uint8_t*)malloc(sizeof(out_alt_interface)) ;
		errors_api_check_if_malloc_succeed(i_out_alt1);
		memcpy(i_out_alt1, out_alt_interface, sizeof(out_alt_interface));
		i_out_alt0[2] = usb_descriptors_alloc_interfaces->interfaces_num[
															curr_interface_idx];
		i_out_alt1[2] = usb_descriptors_alloc_interfaces->interfaces_num[
															curr_interface_idx];
	}

	configure_endpoints(adev, cfg_hndl, i_in_alt1, i_out_alt1);

	if (0 != enable_recording)
	{
		num_of_bytes_per_sample = cfg_hndl->num_of_tx_bytes_per_sample;
		host_in_sample_rate_hz = cfg_hndl->host_in_sample_rate_hz;
		i_in_alt1[FORMAT_OFFSET + 4] = cfg_hndl->num_tx_channels;
		i_in_alt1[FORMAT_OFFSET + 5] = num_of_bytes_per_sample;
		i_in_alt1[FORMAT_OFFSET + 6] = num_of_bytes_per_sample * 8;
		i_in_alt1[FORMAT_OFFSET + 8] = host_in_sample_rate_hz & 0xff;
		i_in_alt1[FORMAT_OFFSET + 9] = (host_in_sample_rate_hz >> 8) & 0xff;
		i_in_alt1[FORMAT_OFFSET + 10] = (host_in_sample_rate_hz >> 16) & 0xff;
		usb_desc_add_interface.interface_desc = i_in_alt0;
		usb_desc_add_interface.interface_desc_size = sizeof(in_interface);
		usb_desc_add_interface.alt_interface_desc = i_in_alt1;
		usb_desc_add_interface.alt_interface_desc_size =
													sizeof(in_alt_interface);
		usb_desc_add_interface.is_hid_interface = 0;
		DEV_IOCTL_1_PARAMS(usb_descriptors_dev,
				USB_DEVICE_DESCRIPTORS_ADD_INTERFACE, &usb_desc_add_interface);
		free(i_in_alt0);
		free(i_in_alt1);
	}

	if (USB_AUDIO_CLASS_NO_PLAYBACK != playback_type)
	{
		num_of_bytes_per_sample = cfg_hndl->num_of_rx_bytes_per_sample;
		host_out_sample_rate_hz = cfg_hndl->host_out_sample_rate_hz;
		i_out_alt1[FORMAT_OFFSET + 4] = cfg_hndl->num_rx_channels;
		i_out_alt1[FORMAT_OFFSET + 5] = num_of_bytes_per_sample;
		i_out_alt1[FORMAT_OFFSET + 6] = num_of_bytes_per_sample * 8;
		i_out_alt1[FORMAT_OFFSET + 8] = host_out_sample_rate_hz & 0xff;
		i_out_alt1[FORMAT_OFFSET + 9] = (host_out_sample_rate_hz >> 8) & 0xff;
		i_out_alt1[FORMAT_OFFSET + 10] = (host_out_sample_rate_hz >> 16) & 0xff;
		usb_desc_add_interface.interface_desc = i_out_alt0;
		usb_desc_add_interface.interface_desc_size = sizeof(out_interface);
		usb_desc_add_interface.alt_interface_desc = i_out_alt1;
		usb_desc_add_interface.alt_interface_desc_size =
													sizeof(out_alt_interface);
		if (USB_AUDIO_CLASS_SYNC_WITH_CORRECTIONS_PLAYBACK == playback_type)
		{
			// remove feedback endpoint
			usb_desc_add_interface.alt_interface_desc_size -= 0x9;
		}
		usb_desc_add_interface.is_hid_interface = 0;
		DEV_IOCTL_1_PARAMS(usb_descriptors_dev,
				USB_DEVICE_DESCRIPTORS_ADD_INTERFACE, &usb_desc_add_interface);
		free(i_out_alt0);
		free(i_out_alt1);
	}
}


void add_audio_class_device(struct dev_desc_t *adev,
		struct usb_audio_class_cfg_t *cfg_hndl,
		struct usb_audio_class_runtime_t *runtime_hndl)
{
	struct register_interfaces_t register_interfaces;
	struct register_interface_t register_interface[3];
	struct usb_descriptors_alloc_interfaces_t usb_descriptors_alloc_interfaces;
	struct dev_desc_t *usb_descriptors_dev;
	struct dev_desc_t *usb_hw;
	uint8_t  playback_type;
	uint8_t  enable_recording;
	uint8_t num_of_interfaces;

	usb_descriptors_dev = cfg_hndl->usb_descriptors_dev;
	playback_type = cfg_hndl->playback_type;
	enable_recording = cfg_hndl->enable_recording;
	num_of_interfaces = 1;// for control interface
	if (0 != enable_recording)
	{
		num_of_interfaces++;
	}
	if (USB_AUDIO_CLASS_NO_PLAYBACK != playback_type)
	{
		num_of_interfaces++;
	}
	usb_descriptors_alloc_interfaces.num_of_interfaces = num_of_interfaces;
	DEV_IOCTL_1_PARAMS(usb_descriptors_dev,
			USB_DEVICE_DESCRIPTORS_ALLOC_INTERFACES_NUMBERS,
			&usb_descriptors_alloc_interfaces);

	update_interfaces_desc(adev, cfg_hndl,
						&usb_descriptors_alloc_interfaces);

	register_interface[0].interface_func = uac_interface_class_request;
	register_interface[1].interface_func = uac_interface_class_request;
	register_interface[2].interface_func = uac_interface_class_request;

	register_interface[0].interfaces_num =
			usb_descriptors_alloc_interfaces.interfaces_num[0];
	num_of_interfaces = 1;
	if (0 != enable_recording)
	{
		register_interface[num_of_interfaces].interfaces_num =
			usb_descriptors_alloc_interfaces.interfaces_num[num_of_interfaces];
		num_of_interfaces++;
	}
	if (USB_AUDIO_CLASS_NO_PLAYBACK != playback_type)
	{
		register_interface[num_of_interfaces].interfaces_num =
			usb_descriptors_alloc_interfaces.interfaces_num[num_of_interfaces];
		num_of_interfaces++;
	}
	register_interfaces.register_interface_arr = register_interface;
	register_interfaces.num_of_interfaces = num_of_interfaces;
	register_interfaces.callback_dev = adev;
	usb_hw = cfg_hndl->usb_hw;
	DEV_IOCTL_1_PARAMS(usb_hw,
			IOCTL_USB_DEVICE_REGISTER_INTERFACES, &register_interfaces);
}
