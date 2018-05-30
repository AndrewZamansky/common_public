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
/*!<Define Audio Class Specific Request */
#define UAC_REQUEST_CODE_UNDEFINED  0x00
#define UAC_SET_CUR                 0x01
#define UAC_GET_CUR                 0x81
#define UAC_SET_MIN                 0x02
#define UAC_GET_MIN                 0x82
#define UAC_SET_MAX                 0x03
#define UAC_GET_MAX                 0x83
#define UAC_SET_RES                 0x04
#define UAC_GET_RES                 0x84
#define UAC_SET_MEM                 0x05
#define UAC_GET_MEM                 0x85
#define UAC_GET_STAT                0xFF

#define MUTE_CONTROL                0x01
#define VOLUME_CONTROL              0x02


/* Define Terminal and Unit ID  */
#define PLAY_IT_ID              0x01
#define PLAY_OT_ID              0x03
#define PLAY_FEATURE_UNITID     0x06
#define REC_IT_ID               0x04
#define REC_OT_ID               0x02
#define REC_FEATURE_UNITID      0x05

#define TO_BE_SET_AUTOMATICALLY   0

#define B3VAL(x) (x&0xFF), ((x >>8) & 0xFF), ((x >>16) & 0xFF)
#define REC_RATE   48000
#define ALT2_PLAY_RATE   48000
#define AUTO_SET_PCKT_SIZE_MSB   0x00
#define AUTO_SET_PCKT_SIZE_LSB   0x00

static uint8_t interface_association_descriptor[]=
{
	/*  Interface Association Descriptor:  */
	0x08,
	0x0B ,  //    bDescriptorType
	TO_BE_SET_AUTOMATICALLY, // bFirstInterface
	0x03    , // bInterfaceCount
	0x01    , // bFunctionClass
	0x00    , // bFunctionSubClass
	0x00    , // bFunctionProtocol
	0x02    ,//  iFunction
};

static uint8_t audio_control_interface[] =
{
    0x09,        // bLength
    0x04,        // bDescriptorType (Interface)
    TO_BE_SET_AUTOMATICALLY,        // bInterfaceNumber 0
    0x00,        // bAlternateSetting
    0x00,        // bNumEndpoints 0
    0x01,        // bInterfaceClass (Audio)
    0x01,        // bInterfaceSubClass (Audio Control)
    0x00,        // bInterfaceProtocol
    0x00,        // iInterface (String Index)

    0x0A,        // bLength
    0x24,        // bDescriptorType (See Next Line)
    0x01,        // bDescriptorSubtype (CS_INTERFACE -> HEADER)
    0x00, 0x01,  // bcdADC 1.00
    0x48, 0x00,  // wTotalLength 72
    0x02,        // binCollection 0x02
    TO_BE_SET_AUTOMATICALLY,        // baInterfaceNr 1
    TO_BE_SET_AUTOMATICALLY,        // baInterfaceNr 2

    0x0C,        // bLength
    0x24,        // bDescriptorType (See Next Line)
    0x02,        // bDescriptorSubtype (CS_INTERFACE -> INPUT_TERMINAL)
    0x01,        // bTerminalID
    0x01, 0x01,  // wTerminalType (USB Streaming)
    0x00,        // bAssocTerminal
    0x02,        // bNrChannels 2
    0x03, 0x00,  // wChannelConfig (Left and Right Front)
    0x00,        // iChannelNames
    0x00,        // iTerminal

    0x0A,        // bLength
    0x24,        // bDescriptorType (See Next Line)
    0x06,        // bDescriptorSubtype (CS_INTERFACE -> FEATURE_UNIT)
    0x06,        // bUnitID
    0x01,        // bSourceID
    0x01,        // bControlSize 1
    0x01, 0x02,  // bmaControls[0] (Mute)
    0x02, 0x00,  // bmaControls[1] (Volume)

    0x09,        // bLength
    0x24,        // bDescriptorType (See Next Line)
    0x03,        // bDescriptorSubtype (CS_INTERFACE -> OUTPUT_TERMINAL)
    0x03,        // bTerminalID
    0x01, 0x03,  // wTerminalType (Speaker)
    0x00,        // bAssocTerminal
    0x06,        // bSourceID
    0x00,        // iTerminal

    0x0C,        // bLength
    0x24,        // bDescriptorType (See Next Line)
    0x02,        // bDescriptorSubtype (CS_INTERFACE -> INPUT_TERMINAL)
    0x04,        // bTerminalID
    0x01, 0x02,  // wTerminalType (Microphone)
    0x00,        // bAssocTerminal
    0x02,        // bNrChannels 2
    0x03, 0x00,  // wChannelConfig (Left and Right Front)
    0x00,        // iChannelNames
    0x00,        // iTerminal

    0x0A,        // bLength
    0x24,        // bDescriptorType (See Next Line)
    0x06,        // bDescriptorSubtype (CS_INTERFACE -> FEATURE_UNIT)
    0x05,        // bUnitID
    0x04,        // bSourceID
    0x01,        // bControlSize 1
    0x01, 0x02,  // bmaControls[0] (Mute)
    0x02, 0x00,  // bmaControls[1] (Volume)

    0x09,        // bLength
    0x24,        // bDescriptorType (See Next Line)
    0x03,        // bDescriptorSubtype (CS_INTERFACE -> OUTPUT_TERMINAL)
    0x02,        // bTerminalID
    0x01, 0x01,  // wTerminalType (USB Streaming)
    0x00,        // bAssocTerminal
    0x05,        // bSourceID
    0x00,        // iTerminal
};

static uint8_t in_interface[] = {
	    0x09,        // bLength
	    0x04,        // bDescriptorType (Interface)
	    TO_BE_SET_AUTOMATICALLY,        // bInterfaceNumber
	    0x00,        // bAlternateSetting
	    0x00,        // bNumEndpoints 0
	    0x01,        // bInterfaceClass (Audio)
	    0x02,        // bInterfaceSubClass (Audio Streaming)
	    0x00,        // bInterfaceProtocol
	    0x00        // iInterface (String Index)
};

static uint8_t in_alt_interface[] = {
		0x09,        // bLength
	    0x04,        // bDescriptorType (Interface)
	    TO_BE_SET_AUTOMATICALLY,        // bInterfaceNumber
	    0x01,        // bAlternateSetting
	    0x01,        // bNumEndpoints 1
	    0x01,        // bInterfaceClass (Audio)
	    0x02,        // bInterfaceSubClass (Audio Streaming)
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
	    0x02,        // bSubFrameSize 2
	    0x10,        // bBitResolution 16
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
	    0x00,        // bmAttributes (None)
	    0x00,        // bLockDelayUnits
	    0x00, 0x00  // wLockDelay 0
};

static uint8_t out_interface[] = {
	    0x09,        // bLength
	    0x04,        // bDescriptorType (Interface)
	    TO_BE_SET_AUTOMATICALLY,        // bInterfaceNumber
	    0x00,        // bAlternateSetting
	    0x00,        // bNumEndpoints 0
	    0x01,        // bInterfaceClass (Audio)
	    0x02,        // bInterfaceSubClass (Audio Streaming)
	    0x00,        // bInterfaceProtocol
	    0x00        // iInterface (String Index)
};

static uint8_t out_alt_interface[] = {
	    0x09,        // bLength
	    0x04,        // bDescriptorType (Interface)
	    TO_BE_SET_AUTOMATICALLY,        // bInterfaceNumber
	    0x01,        // bAlternateSetting
	    0x01,        // bNumEndpoints 1
	    0x01,        // bInterfaceClass (Audio)
	    0x02,        // bInterfaceSubClass (Audio Streaming)
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
	    0x02,        // bSubFrameSize 2
	    0x10,        // bBitResolution 16
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
	    0x80,        // bmAttributes (None)
	    0x00,        // bLockDelayUnits
	    0x00, 0x00  // wLockDelay 0
};


volatile uint8_t g_usbd_RecMute       = 0x01;     /* Record MUTE control. 0 = normal. 1 = MUTE */
volatile int16_t g_usbd_RecVolumeL    = 0x1000;   /* Record left channel volume. Range is -32768 ~ 32767 */
volatile int16_t g_usbd_RecVolumeR    = 0x1000;   /* Record right channel volume. Range is -32768 ~ 32767 */
volatile int16_t g_usbd_RecMaxVolume  = 0x7FFF;
volatile int16_t g_usbd_RecMinVolume  = 0x8000;
volatile int16_t g_usbd_RecResVolume  = 0x400;

volatile uint8_t g_usbd_PlayMute      = 0x01;     /* Play MUTE control. 0 = normal. 1 = MUTE */
volatile int16_t g_usbd_PlayVolumeL   = 0x1000;   /* Play left channel volume. Range is -32768 ~ 32767 */
volatile int16_t g_usbd_PlayVolumeR   = 0x1000;   /* Play right channel volume. Range is -32768 ~ 32767 */
volatile int16_t g_usbd_PlayMaxVolume = 0x7FFF;
volatile int16_t g_usbd_PlayMinVolume = 0x8000;
volatile int16_t g_usbd_PlayResVolume = 0x400;




static void new_audio_received(
		struct dev_desc_t *adev, uint8_t const *buff, size_t size)
{
	struct usb_audio_class_cfg_t *cfg_hndl;
	struct usb_audio_class_runtime_t *runtime_hndl;

	uint8_t	curr_buff_indx;
	uint8_t	next_buff_indx;
	uint8_t *next_buff_status;
	uint8_t *curr_buff_status;
	uint8_t **buffers;
	struct dev_desc_t *callback_dev;
	uint32_t buff_size;
	uint32_t curr_pos_in_buffer;
	uint32_t bytes_to_copy;
	uint8_t *curr_buff;
	int sample_to_skip_or_repeat;
	uint8_t num_of_bytes_per_sample;
	uint16_t num_of_bytes_per_sample_all_channels;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(adev);
	runtime_hndl = DEV_GET_RUNTIME_DATA_POINTER(adev);;
	num_of_bytes_per_sample = cfg_hndl->num_of_bytes_per_sample;
	sample_to_skip_or_repeat = runtime_hndl->sample_to_skip_or_repeat;
	buff_size = cfg_hndl->buff_size;

	// 2 for num of channels
	num_of_bytes_per_sample_all_channels = 2 * num_of_bytes_per_sample;

	if (buff_size < size)
	{
		CRITICAL_ERROR("support only buff_size > size \n");
	}
	if (1 == ( size & 0x01 ))
	{
		CRITICAL_ERROR("odd numbers of bytes \n");
	}

	curr_pos_in_buffer = runtime_hndl->curr_pos_in_buffer;
	curr_buff_indx = runtime_hndl->curr_buff_indx;
	buffers = runtime_hndl->buff;
	curr_buff = buffers[curr_buff_indx];

	if ( 0 < sample_to_skip_or_repeat )
	{
		size = size - num_of_bytes_per_sample_all_channels;
		sample_to_skip_or_repeat--;
	}
	bytes_to_copy = buff_size - curr_pos_in_buffer;
	if (size < bytes_to_copy)
	{
		bytes_to_copy = size;
	}

	memcpy(&curr_buff[curr_pos_in_buffer], buff, bytes_to_copy);
	curr_pos_in_buffer = curr_pos_in_buffer + bytes_to_copy;
	if (( 0 > sample_to_skip_or_repeat) && (buff_size >=
				(curr_pos_in_buffer + num_of_bytes_per_sample_all_channels)))
	{
		memcpy(&curr_buff[
				curr_pos_in_buffer - num_of_bytes_per_sample_all_channels],
				curr_buff, num_of_bytes_per_sample_all_channels);
		curr_pos_in_buffer =
				curr_pos_in_buffer + num_of_bytes_per_sample_all_channels;
		sample_to_skip_or_repeat++;
	}

	if (curr_pos_in_buffer == buff_size)
	{
		curr_pos_in_buffer = 0;
		next_buff_indx = (curr_buff_indx + 1) % USB_AUDIO_CLASS_NUM_OF_BUFFERS;
		curr_buff_status = &runtime_hndl->buff_status[curr_buff_indx];
		next_buff_status = &runtime_hndl->buff_status[next_buff_indx];

		*curr_buff_status =	USB_AUDIO_CLASS_BUFF_RX_RADA_READY;
		if (USB_AUDIO_CLASS_BUFF_IDLE != *next_buff_status)
		{
			//remove for debugging purposes:
			CRITICAL_ERROR("next rx buffer not ready\n");
		}
		*next_buff_status = USB_AUDIO_CLASS_BUFF_IN_PROCCESS;
		runtime_hndl->curr_buff_indx = next_buff_indx;

		callback_dev = cfg_hndl->callback_dev;
		if  (NULL != callback_dev)
		{
			DEV_CALLBACK_0_PARAMS(callback_dev, CALLBACK_NEW_DATA_ARRIVED);
		}

		size = size - bytes_to_copy;
		if (0 != size)
		{
			buff = buff + bytes_to_copy;
			curr_buff = buffers[next_buff_indx];
			memcpy(curr_buff, buff, size);
			curr_pos_in_buffer = size;
		}
	}
	runtime_hndl->curr_pos_in_buffer = curr_pos_in_buffer;
	sample_to_skip_or_repeat = runtime_hndl->sample_to_skip_or_repeat;

}



static uint8_t get_full_rx_buffer(struct usb_audio_class_cfg_t *cfg_hndl,
		struct usb_audio_class_runtime_t *runtime_hndl,
		uint8_t **buff, size_t *buff_size)
{
	uint8_t next_supplied_rx_buffer;
	uint8_t *buffer_state;

	/*
	 * until we not change buff state from IDLE to READY in
	 * interrupt , curr_buff_indx will not be changed
	 */
	next_supplied_rx_buffer = runtime_hndl->next_supplied_rx_buffer ;
	buffer_state = &runtime_hndl->buff_status[next_supplied_rx_buffer];
	if (USB_AUDIO_CLASS_BUFF_RX_RADA_READY == *buffer_state)
	{
		*buff = runtime_hndl->buff[next_supplied_rx_buffer];
		*buff_size = cfg_hndl->buff_size;
		*buffer_state =	USB_AUDIO_CLASS_BUFF_RX_RADA_PROCESSING;
	}
	else
	{
		*buff = NULL;
		*buff_size = 0;
	}
	return 0;
}


static uint8_t release_rx_buffer(struct usb_audio_class_cfg_t *cfg_hndl,
		struct usb_audio_class_runtime_t *runtime_hndl)
{
	uint8_t next_supplied_rx_buffer;

	next_supplied_rx_buffer = runtime_hndl->next_supplied_rx_buffer;
	runtime_hndl->buff_status[next_supplied_rx_buffer] =
									USB_AUDIO_CLASS_BUFF_IDLE;
	next_supplied_rx_buffer =
			(next_supplied_rx_buffer + 1) % USB_AUDIO_CLASS_NUM_OF_BUFFERS;
	runtime_hndl->next_supplied_rx_buffer = next_supplied_rx_buffer;
	return 0;
}






static uint8_t get_mute(
	struct set_request_in_buffer_t *set_request_in_buffer, uint8_t *request)
{
	uint8_t *data;

	if (REC_FEATURE_UNITID == request[5])
	{
		data = (uint8_t*)&g_usbd_RecMute;
	}
	else if (PLAY_FEATURE_UNITID == request[5])
	{
		data = (uint8_t*)&g_usbd_PlayMute;
	}
	else
	{
		return 1;
	}
	set_request_in_buffer->data = data;
	set_request_in_buffer->size = 1;
	return 0;
}


static uint8_t get_volume(
	struct set_request_in_buffer_t *set_request_in_buffer, uint8_t *request)
{
	uint8_t *data;

	if (REC_FEATURE_UNITID == request[5])
	{
		/* Left or right channel */
		if(request[2] == 1)
		{
			data = (uint8_t*)&g_usbd_RecVolumeL;
		}
		else
		{
			data = (uint8_t*)&g_usbd_RecVolumeR;
		}
	}
	else if (PLAY_FEATURE_UNITID == request[5])
	{
		/* Left or right channel */
		if(request[2] == 1)
		{
			data = (uint8_t*)&g_usbd_PlayVolumeL;
		}
		else
		{
			data = (uint8_t*)&g_usbd_PlayVolumeR;
		}
	}
	else
	{
		return 1;
	}
	set_request_in_buffer->data = data;
	set_request_in_buffer->size = 2;
	return 0;
}


static uint8_t get_min_volume(
	struct set_request_in_buffer_t *set_request_in_buffer, uint8_t *request)
{
	uint8_t *data;

	if (REC_FEATURE_UNITID == request[5])
	{
		data = (uint8_t*)&g_usbd_RecMinVolume;
	}
	else if (PLAY_FEATURE_UNITID == request[5])
	{
		data = (uint8_t*)&g_usbd_PlayMinVolume;
	}
	else
	{
		return 1;
	}
	set_request_in_buffer->data = data;
	set_request_in_buffer->size = 2;
	return 0;
}


static uint8_t get_max_volume(
	struct set_request_in_buffer_t *set_request_in_buffer, uint8_t *request)
{
	uint8_t *data;

	if (REC_FEATURE_UNITID == request[5])
	{
		data = (uint8_t*)&g_usbd_RecMaxVolume;
	}
	else if (PLAY_FEATURE_UNITID == request[5])
	{
		data = (uint8_t*)&g_usbd_PlayMaxVolume;
	}
	else
	{
		return 1;
	}
	set_request_in_buffer->data = data;
	set_request_in_buffer->size = 2;
	return 0;
}


static uint8_t get_res_volume(
	struct set_request_in_buffer_t *set_request_in_buffer, uint8_t *request)
{
	uint8_t *data;

	if (REC_FEATURE_UNITID == request[5])
	{
		data = (uint8_t*)&g_usbd_RecResVolume;
	}
	else if (PLAY_FEATURE_UNITID == request[5])
	{
		data = (uint8_t*)&g_usbd_PlayResVolume;
	}
	else
	{
		return 1;
	}
	set_request_in_buffer->data = data;
	set_request_in_buffer->size = 2;
	return 0;
}


/* uac_class_in_request()
 *
 */
static void uac_class_in_request( struct dev_desc_t *usb_hw, uint8_t *request)
{
	struct set_request_in_buffer_t set_request_in_buffer;
	uint8_t ret;

	// Device to host
	switch(request[1])
	{
	case UAC_GET_CUR:
		switch(request[3])
		{
		case MUTE_CONTROL:
			ret = get_mute(&set_request_in_buffer, request);
			break;
		case VOLUME_CONTROL:
			ret = get_volume(&set_request_in_buffer, request);
			break;
		default:
			ret = 1;
			break;
		}
		break;
	case UAC_GET_MIN:
		switch(request[3])
		{
		case VOLUME_CONTROL:
			ret = get_min_volume(&set_request_in_buffer, request);
			break;
		default:
			ret = 1;
			break;
		}
		break;
	case UAC_GET_MAX:
		switch(request[3])
		{
		case VOLUME_CONTROL:
			ret = get_max_volume(&set_request_in_buffer, request);
			break;
		default:
			ret = 1;
			break;
		}
		break;
	case UAC_GET_RES:
		switch(request[3])
		{
		case VOLUME_CONTROL:
			ret = get_res_volume(&set_request_in_buffer, request);
			break;
		default:
			ret = 1;
			break;
		}
		break;
	default:
		ret = 1;
		break;
	}

	if (0 == ret)
	{
		DEV_IOCTL_1_PARAMS(usb_hw,
				IOCTL_USB_DEVICE_SET_REQUEST_IN_BUFFER, &set_request_in_buffer);
	}
	else
	{
		/* Setup error, stall the device */
		DEV_IOCTL_0_PARAMS(usb_hw, IOCTL_USB_DEVICE_SET_SATLL);
	}
}




static uint8_t set_mute(
	struct set_request_out_buffer_t *set_request_out_buffer, uint8_t *request)
{
	uint8_t *data;

	if (REC_FEATURE_UNITID == request[5])
	{
		data = (uint8_t*)&g_usbd_RecMute;
	}
	else if (PLAY_FEATURE_UNITID == request[5])
	{
		data = (uint8_t*)&g_usbd_PlayMute;
	}
	else
	{
		return 1;
	}
	set_request_out_buffer->data = data;
	set_request_out_buffer->size = 1;
	return 0;
}


static uint8_t set_volume(
	struct set_request_out_buffer_t *set_request_out_buffer, uint8_t *request)
{
	uint8_t *data;

	if (REC_FEATURE_UNITID == request[5])
	{
		if (request[2] == 1)
		{
			data = (uint8_t *)&g_usbd_RecVolumeL;
		}
		else
		{
			data = (uint8_t *)&g_usbd_RecVolumeR;
		}
	}
	else if (PLAY_FEATURE_UNITID == request[5])
	{
		if (request[2] == 1)
		{
			data = (uint8_t *)&g_usbd_PlayVolumeL;
		}
		else
		{
			data = (uint8_t *)&g_usbd_PlayVolumeR;
		}
	}
	else
	{
		return 1;
	}

	set_request_out_buffer->data = data;
	set_request_out_buffer->size = 2;
	return 0;
}


/*  uac_class_out_request()
 *
 */
static void uac_class_out_request( struct dev_desc_t *usb_hw, uint8_t *request)
{
	struct set_request_out_buffer_t set_request_out_buffer;
	uint8_t ret;

    switch(request[1])
	{
	case UAC_SET_CUR:
		switch(request[3])
		{
		case MUTE_CONTROL:
			ret = set_mute(&set_request_out_buffer, request);
			break;
		case VOLUME_CONTROL:
			ret = set_volume(&set_request_out_buffer, request);
			break;
		default:
			ret = 1;
			break;
		}
		break;
	default:
		ret = 1;
		break;
	}

    if (0 == ret)
    {
    	DEV_IOCTL_1_PARAMS(usb_hw,
    		IOCTL_USB_DEVICE_SET_REQUEST_OUT_BUFFER, &set_request_out_buffer);
    }
	else
	{
		/* Setup error, stall the device */
		DEV_IOCTL_0_PARAMS(usb_hw, IOCTL_USB_DEVICE_SET_SATLL);
	}
}


static void uac_class_request(struct dev_desc_t *callback_dev, uint8_t *request)
{
	struct usb_audio_class_cfg_t *cfg_hndl;
	struct dev_desc_t *usb_hw;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(callback_dev);
	usb_hw = cfg_hndl->usb_hw;

	if(request[0] & 0x80)    /* request data transfer direction */
	{// from device to host
		uac_class_in_request(usb_hw, request);
	}
	else
	{// from host to device
		uac_class_out_request(usb_hw, request);
	}
}


static void configure_endpoints(struct dev_desc_t *adev,
		struct dev_desc_t *usb_hw,
		struct usb_audio_class_runtime_t *runtime_hndl)
{
	struct set_endpoints_t set_endpoints;
	usb_dev_in_endpoint_callback_func_t   in_func_arr[3];
	uint8_t   endpoints_num_arr[3];
	usb_dev_out_endpoint_callback_func_t   func_arr[3];
	uint8_t    endpoints_type_arr[3];
	uint16_t   max_pckt_sizes[3];
	uint8_t in_endpoint_num;
	uint8_t out_endpoint_num;
	uint8_t endpoint_packet_size;

	endpoint_packet_size = 192;//48000 * 2 * 2 / 1000;

	set_endpoints.num_of_endpoints = 2;
	set_endpoints.endpoints_num_arr = endpoints_num_arr;
	func_arr[0] = NULL;
	func_arr[1] = new_audio_received;
	in_func_arr[0] = NULL;
	in_func_arr[1] = NULL;
	set_endpoints.func_arr = func_arr;
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
	in_alt_interface[27 + 2] = 0x80 | in_endpoint_num;// 0x80 for IN endpoint
	in_alt_interface[27 + 4] = endpoint_packet_size & 0xFF;
	in_alt_interface[27 + 5] = (endpoint_packet_size >> 8) & 0xFF;
	out_alt_interface[27 + 2] = out_endpoint_num;
	out_alt_interface[27 + 4] = endpoint_packet_size & 0xFF;
	out_alt_interface[27 + 5] = (endpoint_packet_size >> 8) & 0xFF;
}


static void update_configuration_desc(struct dev_desc_t *usb_descriptors_dev)
{
	struct usb_descriptors_add_interface_t usb_desc_add_interface;

	DEV_IOCTL_1_PARAMS(usb_descriptors_dev,
			USB_DEVICE_DESCRIPTORS_ADD_INTERFACE_ASSOCIATION_DESCRIPTOR,
			interface_association_descriptor);


	usb_desc_add_interface.interface_desc = audio_control_interface;
	usb_desc_add_interface.interface_desc_size =
								sizeof(audio_control_interface);
	usb_desc_add_interface.alt_interface_desc = NULL;
	DEV_IOCTL_1_PARAMS(usb_descriptors_dev,
			USB_DEVICE_DESCRIPTORS_ADD_INTERFACE, &usb_desc_add_interface);


	usb_desc_add_interface.interface_desc = in_interface;
	usb_desc_add_interface.interface_desc_size = sizeof(in_interface);
	usb_desc_add_interface.alt_interface_desc = in_alt_interface;
	usb_desc_add_interface.alt_interface_desc_size = sizeof(in_alt_interface);
	DEV_IOCTL_1_PARAMS(usb_descriptors_dev,
			USB_DEVICE_DESCRIPTORS_ADD_INTERFACE, &usb_desc_add_interface);

	usb_desc_add_interface.interface_desc = out_interface;
	usb_desc_add_interface.interface_desc_size = sizeof(out_interface);
	usb_desc_add_interface.alt_interface_desc = out_alt_interface;
	usb_desc_add_interface.alt_interface_desc_size = sizeof(out_alt_interface);
	DEV_IOCTL_1_PARAMS(usb_descriptors_dev,
			USB_DEVICE_DESCRIPTORS_ADD_INTERFACE, &usb_desc_add_interface);
}


static void start_audio_class(struct dev_desc_t *adev,
		struct usb_audio_class_cfg_t *cfg_hndl,
		struct usb_audio_class_runtime_t *runtime_hndl)
{
	struct register_interfaces_t register_interfaces;
	struct register_interface_t register_interface[2];
	struct usb_descriptors_alloc_interfaces_t usb_descriptors_alloc_interfaces;
	struct dev_desc_t *usb_descriptors_dev;
	struct dev_desc_t *usb_hw;
	uint32_t buff_size;
	uint8_t i;

	usb_descriptors_dev = cfg_hndl->usb_descriptors_dev;
	usb_descriptors_alloc_interfaces.num_of_interfaces = 3;
	DEV_IOCTL_1_PARAMS(usb_descriptors_dev,
			USB_DEVICE_DESCRIPTORS_ALLOC_INTERFACES_NUMBERS,
			&usb_descriptors_alloc_interfaces);

	interface_association_descriptor[2] =
			usb_descriptors_alloc_interfaces.interfaces_num[0];

	audio_control_interface[2] =
			usb_descriptors_alloc_interfaces.interfaces_num[0];
	audio_control_interface[17] =
			usb_descriptors_alloc_interfaces.interfaces_num[1];
	audio_control_interface[18] =
			usb_descriptors_alloc_interfaces.interfaces_num[2];
	in_interface[2] =
			usb_descriptors_alloc_interfaces.interfaces_num[1];
	in_alt_interface[2] =
			usb_descriptors_alloc_interfaces.interfaces_num[1];
	out_interface[2] =
			usb_descriptors_alloc_interfaces.interfaces_num[2];
	out_alt_interface[2] =
			usb_descriptors_alloc_interfaces.interfaces_num[2];

	usb_hw = cfg_hndl->usb_hw;


	configure_endpoints(adev, usb_hw, runtime_hndl);

	update_configuration_desc(usb_descriptors_dev);

	register_interface[0].interfaces_num =
			usb_descriptors_alloc_interfaces.interfaces_num[0];
	register_interface[0].interface_func = uac_class_request;
	register_interface[1].interfaces_num =
			usb_descriptors_alloc_interfaces.interfaces_num[1];
	register_interface[1].interface_func = uac_class_request;
	register_interface[2].interfaces_num =
			usb_descriptors_alloc_interfaces.interfaces_num[2];
	register_interface[2].interface_func = uac_class_request;

	register_interfaces.num_of_interfaces = 3;
	register_interfaces.register_interface_arr = register_interface;
	register_interfaces.callback_dev = adev;
	DEV_IOCTL_1_PARAMS(usb_hw,
			IOCTL_USB_DEVICE_REGISTER_INTERFACES, &register_interfaces);


	buff_size = cfg_hndl->buff_size;
	for (i = 0; i < USB_AUDIO_CLASS_NUM_OF_BUFFERS; i++)
	{
		runtime_hndl->buff[i] = (uint8_t*)malloc(buff_size);
		runtime_hndl->buff_status[i] = USB_AUDIO_CLASS_BUFF_IDLE ;
	}
}


/**
 * usb_i94xxx_ioctl()
 *
 * return:
 */
uint8_t usb_audio_class_ioctl( struct dev_desc_t *adev, uint8_t aIoctl_num,
		void * aIoctl_param1, void * aIoctl_param2)
{
	struct usb_audio_class_cfg_t *cfg_hndl;
	struct usb_audio_class_runtime_t *runtime_hndl;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(adev);
	runtime_hndl = DEV_GET_RUNTIME_DATA_POINTER(adev);

	switch(aIoctl_num)
	{


	case IOCTL_DEVICE_START :
		start_audio_class(adev, cfg_hndl, runtime_hndl);
		break;

	case USB_AUDIO_CLASS_IOCTL_GET_RX_BUFF :
		get_full_rx_buffer(cfg_hndl,
				runtime_hndl, aIoctl_param1, aIoctl_param2);
		break;

	case USB_AUDIO_CLASS_IOCTL_RELEASE_RX_BUFF :
		release_rx_buffer(cfg_hndl, runtime_hndl);
		break;

	case USB_AUDIO_CLASS_IOCTL_SLOWDOWN_BY_SKIPING_SAMPLES :
		runtime_hndl->sample_to_skip_or_repeat = *(uint32_t*)aIoctl_param1;
		break;

	case USB_AUDIO_CLASS_IOCTL_ACCELERATE_BY_REPEATING_SAMPLES :
		runtime_hndl->sample_to_skip_or_repeat = -(*(uint32_t*)aIoctl_param1);
		break;

	case USB_AUDIO_CLASS_IOCTL_SOFT_RESET :
		runtime_hndl->sample_to_skip_or_repeat = 0;
		break;

	default :
		return 1;
	}
	return 0;
}
