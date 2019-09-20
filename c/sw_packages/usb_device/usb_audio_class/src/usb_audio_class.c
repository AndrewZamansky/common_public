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

/*following line add module to available module list for dynamic device tree*/
#include "usb_audio_class_add_component.h"


enum USB_AUDIO_CLASS_buff_state_t {
	USB_AUDIO_CLASS_BUFF_IDLE,
	USB_AUDIO_CLASS_RX_BUFF_IS_FILLING,
	USB_AUDIO_CLASS_TX_BUFF_IS_FILLING,
	USB_AUDIO_CLASS_TX_BUFF_IS_READY,
	USB_AUDIO_CLASS_RX_BUFF_IS_READY,
	USB_AUDIO_CLASS_RX_BUFF_IS_PROCESSING
};

enum usb_audio_rx_overflow_underflow_state_t {
	USB_AUDIO_CLASS_RX_BUFF_OK,
	USB_AUDIO_CLASS_RX_BUFF_ABOUT_TO_OVERFLOW,
	USB_AUDIO_CLASS_RX_BUFF_ABOUT_TO_UNDERFLOW,
	USB_AUDIO_CLASS_RX_BUFF_OVERFLOWED
};

#define NUM_OF_TX_BUFFERS   5

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
#define SAMPLING_FREQ_CONTROL       0x1


extern void add_audio_class_device(struct dev_desc_t *adev,
		struct usb_audio_class_cfg_t *cfg_hndl,
		struct usb_audio_class_runtime_t *runtime_hndl);

static volatile uint32_t g_usbd_PlaySampleRate = 48000;


/* Record MUTE control. 0 = normal. 1 = MUTE */
static volatile uint8_t g_usbd_RecMute       = 0x01;

/* Play MUTE control. 0 = normal. 1 = MUTE */
static volatile uint8_t g_usbd_PlayMute      = 0x01;

/* Record left channel volume. Range is -32768 ~ 32767 */
static volatile int16_t g_usbd_RecVolumeL    = 0x1000;

/* Record right channel volume. Range is -32768 ~ 32767 */
static volatile int16_t g_usbd_RecVolumeR    = 0x1000;
static volatile int16_t g_usbd_RecMaxVolume  = 0x7FFF;
static volatile int16_t g_usbd_RecMinVolume  = 0x8000;
static volatile int16_t g_usbd_RecResVolume  = 0x400;

/* Play left channel volume. Range is -32768 ~ 32767 */
static volatile int16_t g_usbd_PlayVolumeL   = 0x1000;

/* Play right channel volume. Range is -32768 ~ 32767 */
static volatile int16_t g_usbd_PlayVolumeR   = 0x1000;
static volatile int16_t g_usbd_PlayMaxVolume = 0x7FFF;
static volatile int16_t g_usbd_PlayMinVolume = 0x8000;
static volatile int16_t g_usbd_PlayResVolume = 0x0400;

static uint8_t g_usbd_RecStarted = 0;


#define DEBUG


#define SMOOTH_VALUE    300
static uint16_t skip_repeat_smooth = 0;

volatile uint32_t dbg_out_thr_overflow_cnt = 0;
volatile uint32_t dbg_out_thr_underflow_cnt = 0;
volatile uint32_t dbg_out_overflow_cnt = 0;
volatile uint32_t dbg_out_underflow_cnt = 0;
volatile uint32_t dbg_num_of_data_ready_buffers = 0;
volatile uint32_t dbg_cnt11 = 0;
struct usb_audio_class_runtime_t * dbg_usb_audio_class_runtime_hndl;

#define BUFFER_TO_SKIP   20
static uint8_t skip_first_buffers = BUFFER_TO_SKIP;


static void check_num_of_ready_buffers( uint8_t  *rx_buff_status,
		uint8_t curr_buff_indx, uint8_t num_of_rx_buffers)
{
#ifdef  DEBUG
	uint8_t num_of_data_ready_buffers;
	uint8_t k;
	uint8_t  next_lookup_status;

	num_of_data_ready_buffers = 0;
	for (k = 0; k < num_of_rx_buffers; k++)
	{
		uint8_t indx;
		indx = (curr_buff_indx + k) % num_of_rx_buffers;
		next_lookup_status = rx_buff_status[indx];
		if (USB_AUDIO_CLASS_RX_BUFF_IS_READY == next_lookup_status)
		{
			num_of_data_ready_buffers++;
		}

	}
	dbg_num_of_data_ready_buffers = num_of_data_ready_buffers;
#endif

}


static uint8_t test_overflow_threshold(
	struct usb_audio_class_runtime_t *runtime_hndl,
	uint8_t num_of_rx_buffers, uint8_t num_of_rx_over_under_flow_buffers_thr,
	uint32_t host_out_sample_rate_hz)
{
	uint8_t  *rx_buff_status;
	uint8_t  curr_buff_indx;
	uint8_t  next_lookup_indx;
	uint8_t  next_lookup_status;
	uint8_t  prev_lookup_status;
	uint8_t  curr_buff_status;

	curr_buff_indx = runtime_hndl->curr_buff_indx;
	rx_buff_status = runtime_hndl->rx_buff_status;
	curr_buff_status = rx_buff_status[curr_buff_indx];
	if ((USB_AUDIO_CLASS_RX_BUFF_IS_FILLING != curr_buff_status) &&
			((USB_AUDIO_CLASS_BUFF_IDLE != curr_buff_status) ))
	{
		dbg_cnt11++;
		return USB_AUDIO_CLASS_RX_BUFF_OVERFLOWED;
	}

	next_lookup_indx = (curr_buff_indx + num_of_rx_over_under_flow_buffers_thr)
															% num_of_rx_buffers;
	next_lookup_status = rx_buff_status[next_lookup_indx];
	if (USB_AUDIO_CLASS_BUFF_IDLE != next_lookup_status)
	{
		dbg_out_thr_overflow_cnt++;
		// frame is 1ms so sample rate *1000
		runtime_hndl->usb_feedback_sample_rate =
			host_out_sample_rate_hz - (HOST_OUT_MAX_ADDITIONAL_SAMPLES * 1000);
		return USB_AUDIO_CLASS_RX_BUFF_ABOUT_TO_OVERFLOW;
	}

	prev_lookup_status = (curr_buff_indx + num_of_rx_buffers -
			num_of_rx_over_under_flow_buffers_thr) % num_of_rx_buffers;
	prev_lookup_status = rx_buff_status[prev_lookup_status];
	if (USB_AUDIO_CLASS_RX_BUFF_IS_READY != prev_lookup_status)
	{
		dbg_out_thr_underflow_cnt++;
		// frame is 1ms so sample rate *1000
		runtime_hndl->usb_feedback_sample_rate =
			host_out_sample_rate_hz + (HOST_OUT_MAX_ADDITIONAL_SAMPLES * 1000);
		return USB_AUDIO_CLASS_RX_BUFF_ABOUT_TO_UNDERFLOW;
	}

	runtime_hndl->usb_feedback_sample_rate = host_out_sample_rate_hz;
	return USB_AUDIO_CLASS_RX_BUFF_OK;
}


static  void copy_to_current_rx_buffer(struct usb_audio_class_cfg_t *cfg_hndl,
		struct usb_audio_class_runtime_t *runtime_hndl,
		uint8_t const **p_buff, size_t  *size)
{
	uint8_t  buff_overflow_threshold_status;
	uint8_t *all_rx_buff_status;
	uint8_t curr_buff_indx;
	uint8_t *curr_buff;
	uint16_t num_of_bytes_per_sample_all_channels;
	uint8_t num_of_bytes_per_sample;
	uint32_t get_rx_buff_size;
	uint32_t curr_pos_in_rx_buffer;
	uint32_t bytes_to_copy;
	uint32_t new_size;
	uint8_t	next_buff_indx;
	uint8_t num_of_rx_buffers;
	uint8_t  playback_type;

	curr_buff_indx = runtime_hndl->curr_buff_indx;
	all_rx_buff_status = runtime_hndl->rx_buff_status;
	num_of_rx_buffers = cfg_hndl->num_of_rx_buffers;
	buff_overflow_threshold_status = test_overflow_threshold(runtime_hndl,
			num_of_rx_buffers, cfg_hndl->num_of_rx_over_under_flow_buffers_thr,
			cfg_hndl->host_out_sample_rate_hz);
	if (USB_AUDIO_CLASS_RX_BUFF_OVERFLOWED  == buff_overflow_threshold_status)
	{
		dbg_out_overflow_cnt++;
		*size = 0;
		return ;
	}

	new_size = *size;
	playback_type = cfg_hndl->playback_type;
	num_of_bytes_per_sample = cfg_hndl->num_of_rx_bytes_per_sample;
	if ((USB_AUDIO_CLASS_SYNC_WITH_CORRECTIONS_PLAYBACK == playback_type) &&
						(0 != ( new_size % num_of_bytes_per_sample )))
	{
		CRITICAL_ERROR("odd numbers of bytes not supported \n");
	}

	num_of_bytes_per_sample_all_channels =
						cfg_hndl->num_rx_channels * num_of_bytes_per_sample;

	if ((USB_AUDIO_CLASS_RX_BUFF_ABOUT_TO_OVERFLOW ==
											buff_overflow_threshold_status) &&
		(USB_AUDIO_CLASS_SYNC_WITH_CORRECTIONS_PLAYBACK == playback_type)  &&
		(SMOOTH_VALUE <= skip_repeat_smooth++))
	{
		// skip sample
		new_size -= num_of_bytes_per_sample_all_channels;
	}

	get_rx_buff_size = cfg_hndl->get_rx_buff_size;
	curr_pos_in_rx_buffer = runtime_hndl->curr_pos_in_rx_buffer;
	bytes_to_copy = get_rx_buff_size - curr_pos_in_rx_buffer;
	if (new_size < bytes_to_copy)
	{
		bytes_to_copy = new_size;
	}

	curr_buff = runtime_hndl->rx_buff[curr_buff_indx];
	all_rx_buff_status[curr_buff_indx] = USB_AUDIO_CLASS_RX_BUFF_IS_FILLING;
	memcpy(&curr_buff[curr_pos_in_rx_buffer], *p_buff, bytes_to_copy);
	curr_pos_in_rx_buffer = curr_pos_in_rx_buffer + bytes_to_copy;

	if ((USB_AUDIO_CLASS_RX_BUFF_ABOUT_TO_OVERFLOW ==
										buff_overflow_threshold_status) &&
		(USB_AUDIO_CLASS_SYNC_WITH_CORRECTIONS_PLAYBACK == playback_type) &&
		(SMOOTH_VALUE <= skip_repeat_smooth++) &&
		(get_rx_buff_size >=
				(curr_pos_in_rx_buffer + num_of_bytes_per_sample_all_channels)))
	{
		skip_repeat_smooth = 0;
		// duplicate sample
		memcpy(&curr_buff[curr_pos_in_rx_buffer],
			&curr_buff[curr_pos_in_rx_buffer -
					num_of_bytes_per_sample_all_channels],
			num_of_bytes_per_sample_all_channels);
		curr_pos_in_rx_buffer += num_of_bytes_per_sample_all_channels;
	}

	if (curr_pos_in_rx_buffer == get_rx_buff_size)
	{
		curr_pos_in_rx_buffer = 0;
		all_rx_buff_status[curr_buff_indx] = USB_AUDIO_CLASS_RX_BUFF_IS_READY;
		next_buff_indx = (curr_buff_indx + 1) % num_of_rx_buffers;
		runtime_hndl->curr_buff_indx = next_buff_indx;
	}
	runtime_hndl->curr_pos_in_rx_buffer = curr_pos_in_rx_buffer;

	*p_buff = *p_buff + bytes_to_copy;
	*size = new_size - bytes_to_copy;
}


void new_usb_audio_received(
		struct dev_desc_t *adev, uint8_t const *buff, size_t size)
{
	struct usb_audio_class_cfg_t *cfg_hndl;
	struct usb_audio_class_runtime_t *runtime_hndl;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(adev);
	runtime_hndl = DEV_GET_RUNTIME_DATA_POINTER(adev);

	if (skip_first_buffers)
	{
		skip_first_buffers--;
		return;
	}

	while (0 != size)
	{
		copy_to_current_rx_buffer(cfg_hndl, runtime_hndl, &buff, &size);
	}

	check_num_of_ready_buffers( runtime_hndl->rx_buff_status,
			runtime_hndl->curr_buff_indx, cfg_hndl->num_of_rx_buffers);
}


uint32_t volatile dbg_in_underflow_cnt = 0;
size_t  volatile dbg_available_data_size = 0;
size_t  volatile dbg_copy_size = 0;
size_t  volatile dbg_in_thr_overflow_cnt = 0;
size_t  volatile dbg_in_thr_underflow_cnt = 0;

void new_usb_audio_requested(struct dev_desc_t *adev)
{
	struct usb_audio_class_cfg_t *cfg_hndl;
	struct usb_audio_class_runtime_t *runtime_hndl;
	size_t  curr_write_pos_in_tx_buffer;
	size_t  curr_read_pos_in_tx_buffer;
	size_t  available_data_size;
	size_t   tx_buff_size;
	size_t   get_tx_buff_size;
	uint8_t  *tx_buff;
	uint8_t  *tx_pckt_buff;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(adev);
	runtime_hndl = DEV_GET_RUNTIME_DATA_POINTER(adev);

	get_tx_buff_size = cfg_hndl->get_tx_buff_size;
	tx_buff_size = get_tx_buff_size * NUM_OF_TX_BUFFERS;
	curr_write_pos_in_tx_buffer = runtime_hndl->curr_write_pos_in_tx_buffer;
	curr_read_pos_in_tx_buffer = runtime_hndl->curr_read_pos_in_tx_buffer;

	if (curr_write_pos_in_tx_buffer >= curr_read_pos_in_tx_buffer)
	{
		available_data_size =
				curr_write_pos_in_tx_buffer - curr_read_pos_in_tx_buffer;
	}
	else
	{
		available_data_size = tx_buff_size -
				(curr_read_pos_in_tx_buffer - curr_write_pos_in_tx_buffer);
	}

	dbg_available_data_size = available_data_size;
	if (available_data_size > get_tx_buff_size)
	{
		struct set_data_to_in_endpoint_t  data_to_endpoint;
		size_t  data_to_copy;
		uint16_t num_of_bytes_per_sample_all_channels;

		num_of_bytes_per_sample_all_channels =
			cfg_hndl->num_tx_channels * cfg_hndl->num_of_tx_bytes_per_sample;

		data_to_copy = runtime_hndl->normal_host_in_pckt_size;
		if (available_data_size < (2 * get_tx_buff_size))
		{// reduce speed(clock)
			data_to_copy -= (num_of_bytes_per_sample_all_channels);
			dbg_in_thr_underflow_cnt++;
		}
		else if (available_data_size >
					((NUM_OF_TX_BUFFERS - 2) * get_tx_buff_size))
		{// increase speed(clock)
			data_to_copy += (num_of_bytes_per_sample_all_channels);
			dbg_in_thr_overflow_cnt++;
		}
		dbg_copy_size = data_to_copy;
		tx_buff = runtime_hndl->tx_buff;
		tx_pckt_buff = runtime_hndl->tx_pckt_buff;
		data_to_endpoint.endpoint_num = runtime_hndl->in_endpoint_num;
		data_to_endpoint.data = tx_pckt_buff;
		data_to_endpoint.size = data_to_copy;
		while (data_to_copy--)
		{
			*tx_pckt_buff++ = tx_buff[curr_read_pos_in_tx_buffer++];

			if (curr_read_pos_in_tx_buffer == tx_buff_size)
			{
				curr_read_pos_in_tx_buffer = 0;
			}
		}
		DEV_IOCTL_1_PARAMS(cfg_hndl->usb_hw,
				IOCTL_USB_DEVICE_SENT_DATA_TO_IN_ENDPOINT, &data_to_endpoint);
		runtime_hndl->curr_read_pos_in_tx_buffer = curr_read_pos_in_tx_buffer;
	}
	else
	{
		/* Setup error, stall the device */
		DEV_IOCTL_0_PARAMS(cfg_hndl->usb_hw, IOCTL_USB_DEVICE_SET_STALL);
		dbg_in_underflow_cnt++;
	}
}


void new_sample_rate_requested(struct dev_desc_t *adev)
{
	struct usb_audio_class_cfg_t *cfg_hndl;
	struct usb_audio_class_runtime_t *runtime_hndl;
	struct set_data_to_in_endpoint_t  data_to_endpoint;
	uint8_t  tx_buff[3];
	uint32_t usb_feedback_sample_rate;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(adev);
	runtime_hndl = DEV_GET_RUNTIME_DATA_POINTER(adev);

	usb_feedback_sample_rate = runtime_hndl->usb_feedback_sample_rate;
	/* reported sample rate is in format 10.14 and number of samples per frame
	 * for example 48k : val = 0x0c0000
	*/
	tx_buff[0] = 0x00;
	tx_buff[1] = ((usb_feedback_sample_rate / 1000) & 0x3) << 6;
	tx_buff[2] = (usb_feedback_sample_rate / 1000) >> 2;
	data_to_endpoint.endpoint_num = runtime_hndl->in_feedback_endpoint_num;
	data_to_endpoint.data = tx_buff;
	data_to_endpoint.size = 3;
	DEV_IOCTL_1_PARAMS(cfg_hndl->usb_hw,
			IOCTL_USB_DEVICE_SENT_DATA_TO_IN_ENDPOINT, &data_to_endpoint);
}

static uint8_t rx_was_stoped = 1;

static uint8_t get_full_rx_buffer(struct usb_audio_class_cfg_t *cfg_hndl,
		struct usb_audio_class_runtime_t *runtime_hndl,
		uint8_t **buff, size_t *get_rx_buff_size)
{
	uint8_t next_supplied_rx_buffer;
	uint8_t *buffer_state;

	/*
	 * until we not change buff state from IDLE to READY in
	 * interrupt , curr_buff_indx will not be changed
	 */
	next_supplied_rx_buffer = runtime_hndl->next_supplied_rx_buffer ;
	buffer_state = &runtime_hndl->rx_buff_status[next_supplied_rx_buffer];
	if (USB_AUDIO_CLASS_RX_BUFF_IS_READY == *buffer_state)
	{
		if (1 == rx_was_stoped)
		{
			uint8_t look_ahead_supplied_rx_buffer_indx;
			uint8_t *look_ahead_buffer_state;

			// strart to give buffers after half of them are ready to use
			look_ahead_supplied_rx_buffer_indx = (next_supplied_rx_buffer +
					cfg_hndl->num_of_rx_over_under_flow_buffers_thr) %
												cfg_hndl->num_of_rx_buffers;
			look_ahead_buffer_state = &runtime_hndl->
							rx_buff_status[look_ahead_supplied_rx_buffer_indx];
			if (USB_AUDIO_CLASS_RX_BUFF_IS_READY != *look_ahead_buffer_state)
			{
				*buff = NULL;
				*get_rx_buff_size = 0;
				dbg_out_underflow_cnt++;
				return 0;
			}
		}
		*buff = runtime_hndl->rx_buff[next_supplied_rx_buffer];
		*get_rx_buff_size = cfg_hndl->get_rx_buff_size;
		*buffer_state =	USB_AUDIO_CLASS_RX_BUFF_IS_PROCESSING;
		rx_was_stoped = 0;
	}
	else
	{
		*buff = NULL;
		*get_rx_buff_size = 0;
		dbg_out_underflow_cnt++;
		if (0 == rx_was_stoped)
		{
			skip_first_buffers = BUFFER_TO_SKIP;
		}
		rx_was_stoped = 1;
	}
	return 0;
}


static uint8_t release_rx_buffer(struct usb_audio_class_cfg_t *cfg_hndl,
		struct usb_audio_class_runtime_t *runtime_hndl)
{
	uint8_t next_supplied_rx_buffer;
	uint8_t  *rx_buff_status;

	next_supplied_rx_buffer = runtime_hndl->next_supplied_rx_buffer;
	rx_buff_status = &runtime_hndl->rx_buff_status[next_supplied_rx_buffer];
	if (USB_AUDIO_CLASS_RX_BUFF_IS_PROCESSING != *rx_buff_status)
	{
		return 1;
	}
	*rx_buff_status = USB_AUDIO_CLASS_BUFF_IDLE;
	next_supplied_rx_buffer =
			(next_supplied_rx_buffer + 1) % cfg_hndl->num_of_rx_buffers;
	runtime_hndl->next_supplied_rx_buffer = next_supplied_rx_buffer;
	return 0;
}


volatile uint32_t dbg_in_overflow_cnt =0;
static uint8_t get_empty_tx_buffer(struct usb_audio_class_cfg_t *cfg_hndl,
		struct usb_audio_class_runtime_t *runtime_hndl,
		uint8_t **buff, size_t *ret_buff_size)
{
	size_t  curr_write_pos_in_tx_buffer;
	size_t  next_write_pos_in_tx_buffer;
	size_t  curr_read_pos_in_tx_buffer;
	size_t   get_tx_buff_size;
	size_t   tx_buff_size;
	uint8_t  there_is_space_for_write;

	if ((0 == cfg_hndl->enable_recording) ||
			(NULL == runtime_hndl->tx_buff))
	{
		dbg_in_overflow_cnt++;
		*buff = NULL;
		*ret_buff_size = 0;
		runtime_hndl->tx_buffer_was_supplied = 0;
		return 1;
	}


	if (1 == runtime_hndl->tx_buffer_was_supplied)
	{
		dbg_in_overflow_cnt++;
		*buff = NULL;
		*ret_buff_size = 0;
		return 1;
	}

	curr_write_pos_in_tx_buffer = runtime_hndl->curr_write_pos_in_tx_buffer;
	curr_read_pos_in_tx_buffer = runtime_hndl->curr_read_pos_in_tx_buffer;
	get_tx_buff_size = cfg_hndl->get_tx_buff_size;
	tx_buff_size = get_tx_buff_size * NUM_OF_TX_BUFFERS;

	next_write_pos_in_tx_buffer =
			curr_write_pos_in_tx_buffer + get_tx_buff_size;
	if (next_write_pos_in_tx_buffer == tx_buff_size)
	{
		next_write_pos_in_tx_buffer = 0;
	}

	there_is_space_for_write = 0;
	if (curr_write_pos_in_tx_buffer >= curr_read_pos_in_tx_buffer)
	{
		// first check if there will be NO wrap in write buffer pointer
		if ((next_write_pos_in_tx_buffer >= curr_write_pos_in_tx_buffer) ||
			 (next_write_pos_in_tx_buffer < curr_read_pos_in_tx_buffer))
		{
			there_is_space_for_write = 1;
		}
	}
	else
	{
		// first check if there will BE wrap of write buffer pointer
		if ((next_write_pos_in_tx_buffer >= curr_write_pos_in_tx_buffer) &&
			 (next_write_pos_in_tx_buffer < curr_read_pos_in_tx_buffer))
		{
			there_is_space_for_write = 1;
		}

	}
	/* first check if no wrapping occurred*/
	if (there_is_space_for_write)
	{
		*buff = &runtime_hndl->tx_buff[curr_write_pos_in_tx_buffer];
		*ret_buff_size = get_tx_buff_size;
	}
	else
	{
		dbg_in_overflow_cnt++;
		*buff = NULL;
		*ret_buff_size = 0;
		runtime_hndl->tx_buffer_was_supplied = 0;
		return 1;
	}
	runtime_hndl->tx_buffer_was_supplied = 1;

	return 0;
}


static uint8_t release_tx_buffer(struct usb_audio_class_cfg_t *cfg_hndl,
		struct usb_audio_class_runtime_t *runtime_hndl)
{
	size_t  curr_write_pos_in_tx_buffer;
	size_t  next_write_pos_in_tx_buffer;
	size_t   tx_buff_size;
	size_t   get_tx_buff_size;

	if (0 == g_usbd_RecStarted)
	{
		struct set_data_to_in_endpoint_t  data_to_endpoint;

		data_to_endpoint.endpoint_num = runtime_hndl->in_endpoint_num;
		data_to_endpoint.data = NULL;
		data_to_endpoint.size = 0;

		DEV_IOCTL_1_PARAMS(cfg_hndl->usb_hw,
				IOCTL_USB_DEVICE_SENT_DATA_TO_IN_ENDPOINT, &data_to_endpoint);
		g_usbd_RecStarted = 1;
	}

	if ((NULL == runtime_hndl->tx_buff) ||
			( 0 == runtime_hndl->tx_buffer_was_supplied))
	{
		return 0;
	}
	runtime_hndl->tx_buffer_was_supplied = 0;
	get_tx_buff_size = cfg_hndl->get_tx_buff_size;
	tx_buff_size = get_tx_buff_size * NUM_OF_TX_BUFFERS;
	curr_write_pos_in_tx_buffer = runtime_hndl->curr_write_pos_in_tx_buffer;
	next_write_pos_in_tx_buffer =
			curr_write_pos_in_tx_buffer + get_tx_buff_size;
	if (next_write_pos_in_tx_buffer == tx_buff_size)
	{
		next_write_pos_in_tx_buffer = 0;
	}
	runtime_hndl->curr_write_pos_in_tx_buffer =
											next_write_pos_in_tx_buffer;

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


/* uac_class_interface_in_request()
 *
 */
static void uac_class_interface_in_request(
							struct dev_desc_t *usb_hw, uint8_t *request)
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
		DEV_IOCTL_0_PARAMS(usb_hw, IOCTL_USB_DEVICE_SET_STALL);
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


/*  uac_class_interface_out_request()
 *
 */
static void uac_class_interface_out_request(
				struct dev_desc_t *usb_hw, uint8_t *request)
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
		DEV_IOCTL_0_PARAMS(usb_hw, IOCTL_USB_DEVICE_SET_STALL);
	}
}


void uac_interface_class_request(
		struct dev_desc_t *callback_dev, uint8_t *request)
{
	struct usb_audio_class_cfg_t *cfg_hndl;
	struct dev_desc_t *usb_hw;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(callback_dev);
	usb_hw = cfg_hndl->usb_hw;

	if(request[0] & 0x80)    /* request data transfer direction */
	{// from device to host
		uac_class_interface_in_request(usb_hw, request);
	}
	else
	{// from host to device
		uac_class_interface_out_request(usb_hw, request);
	}
}


/*  uac_class_endpoint_out_request()
 *
 */
static void uac_class_endpoint_out_request( struct dev_desc_t *usb_hw,
		struct usb_audio_class_runtime_t *runtime_hndl, uint8_t *request)
{
	struct set_request_out_buffer_t set_request_out_buffer;

	switch(request[1])
	{
	case UAC_SET_CUR:
		if ((request[3] == SAMPLING_FREQ_CONTROL) &&
				(request[4] == runtime_hndl->out_endpoint_num))
		{
			set_request_out_buffer.data = (uint8_t*)&g_usbd_PlaySampleRate;
			set_request_out_buffer.size = 3;
			DEV_IOCTL_1_PARAMS(usb_hw, IOCTL_USB_DEVICE_SET_REQUEST_OUT_BUFFER,
				&set_request_out_buffer);
			return;
		}
		break;
	default:
		break;
	}
	DEV_IOCTL_0_PARAMS(usb_hw, IOCTL_USB_DEVICE_SET_STALL);
}


/*  uac_class_endpoint_in_request()
 *
 */
static void uac_class_endpoint_in_request( struct dev_desc_t *usb_hw,
		struct usb_audio_class_runtime_t *runtime_hndl, uint8_t *request)
{
	struct set_request_in_buffer_t set_request_in_buffer;

	switch(request[1])
	{
	case UAC_GET_CUR:
		if ((request[3] == SAMPLING_FREQ_CONTROL) &&
				(request[4] == runtime_hndl->out_endpoint_num))
		{
			set_request_in_buffer.data = (uint8_t*)&g_usbd_PlaySampleRate;
			set_request_in_buffer.size = 3;
			DEV_IOCTL_1_PARAMS(usb_hw, IOCTL_USB_DEVICE_SET_REQUEST_IN_BUFFER,
				&set_request_in_buffer);
			return;
		}
		break;
	default:
		break;
	}
	DEV_IOCTL_0_PARAMS(usb_hw, IOCTL_USB_DEVICE_SET_STALL);
}


void uac_endpoint_class_request(
		struct dev_desc_t *callback_dev, uint8_t *request)
{
	struct usb_audio_class_runtime_t *runtime_hndl;
	struct usb_audio_class_cfg_t *cfg_hndl;
	struct dev_desc_t *usb_hw;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(callback_dev);
	runtime_hndl = DEV_GET_RUNTIME_DATA_POINTER(callback_dev);
	usb_hw = cfg_hndl->usb_hw;

	if(request[0] & 0x80) // Device to host
	{
		uac_class_endpoint_in_request(usb_hw, runtime_hndl, request);
	}
	else // Host to device
	{
		uac_class_endpoint_out_request(usb_hw, runtime_hndl, request);
	}

}


static void init_audio_host_out(struct usb_audio_class_cfg_t *cfg_hndl,
		struct usb_audio_class_runtime_t *runtime_hndl)
{
	uint8_t *new_buff;
	uint32_t get_rx_buff_size;
	uint32_t i;
	uint8_t num_of_rx_buffers;
	uint8_t  **rx_buff;
	uint8_t  *rx_buff_status;

	runtime_hndl->usb_feedback_sample_rate = cfg_hndl->host_out_sample_rate_hz;


	num_of_rx_buffers = cfg_hndl->num_of_rx_buffers;
	get_rx_buff_size = cfg_hndl->get_rx_buff_size;
	rx_buff = (uint8_t **)malloc(num_of_rx_buffers * sizeof(uint8_t*));
	errors_api_check_if_malloc_succeed(rx_buff);
	runtime_hndl->rx_buff = rx_buff;

	rx_buff_status = (uint8_t *)malloc(num_of_rx_buffers);
	errors_api_check_if_malloc_succeed(rx_buff_status);
	runtime_hndl->rx_buff_status = rx_buff_status;

	for (i = 0; i < num_of_rx_buffers; i++)
	{
		new_buff = (uint8_t*)malloc(get_rx_buff_size);
		errors_api_check_if_malloc_succeed(new_buff);
		rx_buff[i] = new_buff;
		rx_buff_status[i] = USB_AUDIO_CLASS_BUFF_IDLE ;
	}
}


static void init_audio_host_in(struct usb_audio_class_cfg_t *cfg_hndl,
		struct usb_audio_class_runtime_t *runtime_hndl)
{
	uint8_t *new_buff;
	uint16_t normal_host_in_pckt_size;
	uint16_t all_tx_channels_sample_size;
	uint32_t max_host_in_pckt_size;

	all_tx_channels_sample_size =
			cfg_hndl->num_of_tx_bytes_per_sample * cfg_hndl->num_tx_channels;
	normal_host_in_pckt_size = (cfg_hndl->host_in_sample_rate_hz / 1000) *
												all_tx_channels_sample_size;
	runtime_hndl->normal_host_in_pckt_size = normal_host_in_pckt_size;

	max_host_in_pckt_size =
			normal_host_in_pckt_size + all_tx_channels_sample_size;
	new_buff = (uint8_t*)malloc(max_host_in_pckt_size);
	errors_api_check_if_malloc_succeed(new_buff);
	runtime_hndl->tx_pckt_buff = new_buff;

	runtime_hndl->curr_write_pos_in_tx_buffer = 0;
	runtime_hndl->curr_read_pos_in_tx_buffer = 0;

	// init tx_buff last, as it serves for detecting if host_in is initialized
	new_buff = (uint8_t*)malloc(cfg_hndl->get_tx_buff_size * NUM_OF_TX_BUFFERS);
	errors_api_check_if_malloc_succeed(new_buff);
	runtime_hndl->tx_buff = new_buff;
}


static void start_audio_class(struct dev_desc_t *adev,
		struct usb_audio_class_cfg_t *cfg_hndl,
		struct usb_audio_class_runtime_t *runtime_hndl)
{
	dbg_usb_audio_class_runtime_hndl = runtime_hndl;

	if (0 != cfg_hndl->enable_recording)
	{
		init_audio_host_in(cfg_hndl, runtime_hndl);
	}

	if (USB_AUDIO_CLASS_NO_PLAYBACK != cfg_hndl->playback_type)
	{
		init_audio_host_out(cfg_hndl, runtime_hndl);
	}

	add_audio_class_device(adev, cfg_hndl, runtime_hndl);

	if (USB_AUDIO_CLASS_NO_PLAYBACK != cfg_hndl->playback_type)
	{
		struct set_data_to_in_endpoint_t  data_to_endpoint;

		data_to_endpoint.endpoint_num = runtime_hndl->in_feedback_endpoint_num;
		data_to_endpoint.data = NULL;
		data_to_endpoint.size = 0;
		DEV_IOCTL_1_PARAMS(cfg_hndl->usb_hw,
				IOCTL_USB_DEVICE_SENT_DATA_TO_IN_ENDPOINT, &data_to_endpoint);
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

	case USB_AUDIO_CLASS_IOCTL_GET_EMPTY_TX_BUFF :
		return get_empty_tx_buffer(cfg_hndl,
				runtime_hndl,  aIoctl_param1, aIoctl_param2);
		break;

	case USB_AUDIO_CLASS_IOCTL_RELEASE_TX_BUFF :
		release_tx_buffer(cfg_hndl, runtime_hndl);
		break;
	default :
		return 1;
	}
	return 0;
}
