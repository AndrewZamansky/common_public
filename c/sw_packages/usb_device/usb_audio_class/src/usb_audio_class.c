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
#include "os_wrapper.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "dev_management_api.h"

#include "usb_device_api.h"
#include "usb_audio_class_api.h"
#include "usb_audio_class.h"

//#define DEBUG

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

extern void add_audio_class_device(struct dev_desc_t *adev,
		struct usb_audio_class_cfg_t *cfg_hndl,
		struct usb_audio_class_runtime_t *runtime_hndl);


#define SMOOTH_VALUE    300
static uint16_t skip_repeat_smooth = 0;

static uint8_t g_usbd_RecStarted = 0;


#ifdef  DEBUG
#define DBG_MAX_BUFF_SIZE    32

volatile uint32_t dbg_out_thr_overflow_cnt = 0;
volatile uint32_t dbg_out_thr_underflow_cnt = 0;
volatile uint32_t dbg_out_overflow_cnt = 0;
volatile uint32_t dbg_out_underflow_cnt = 0;
volatile uint32_t dbg_num_of_data_ready_buffers = 0;
volatile uint32_t dbg_cnt11 = 0;

volatile uint32_t dbg_in_underflow_cnt = 0;
volatile uint32_t dbg_available_data_size = 0;
volatile uint32_t dbg_copy_size = 0;
volatile uint32_t dbg_in_thr_overflow_cnt = 0;
volatile uint32_t dbg_in_thr_underflow_cnt = 0;

volatile uint32_t dbg_in_overflow_cnt =0;

static uint8_t dbg_buffer[DBG_MAX_BUFF_SIZE] = {0};

struct usb_audio_class_runtime_t * dbg_usb_audio_class_runtime_hndl;
#endif

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
#ifdef  DEBUG
		dbg_cnt11++;
#endif
		return USB_AUDIO_CLASS_RX_BUFF_OVERFLOWED;
	}

	next_lookup_indx = (curr_buff_indx + num_of_rx_over_under_flow_buffers_thr)
															% num_of_rx_buffers;
	next_lookup_status = rx_buff_status[next_lookup_indx];
	if (USB_AUDIO_CLASS_BUFF_IDLE != next_lookup_status)
	{
#ifdef  DEBUG
		dbg_out_thr_overflow_cnt++;
#endif
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
#ifdef  DEBUG
		dbg_out_thr_underflow_cnt++;
#endif
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
#ifdef  DEBUG
		dbg_out_overflow_cnt++;
#endif
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

#ifdef DEBUG
	if(DBG_MAX_BUFF_SIZE < bytes_to_copy)
	{
		memcpy(dbg_buffer, *p_buff, DBG_MAX_BUFF_SIZE);
	}
	else
	{
		memcpy(dbg_buffer, *p_buff, bytes_to_copy);
	}
#endif


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

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(usb_audio_class, adev);
	runtime_hndl = DEV_GET_RUNTIME_DATA_POINTER(usb_audio_class, adev);

	if (skip_first_buffers)
	{
		skip_first_buffers--;
		return;
	}

	//perform shift protection in case corrupted USB packet has wrong size
	// by always align to frame size
	size -= size % cfg_hndl->frame_size;

	while (0 != size)
	{
		copy_to_current_rx_buffer(cfg_hndl, runtime_hndl, &buff, &size);
	}

	check_num_of_ready_buffers( runtime_hndl->rx_buff_status,
			runtime_hndl->curr_buff_indx, cfg_hndl->num_of_rx_buffers);
}


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

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(usb_audio_class, adev);
	runtime_hndl = DEV_GET_RUNTIME_DATA_POINTER(usb_audio_class, adev);

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
#ifdef  DEBUG
	dbg_available_data_size = available_data_size;
#endif
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
#ifdef  DEBUG
			dbg_in_thr_underflow_cnt++;
#endif
		}
		else if (available_data_size >
					((NUM_OF_TX_BUFFERS - 2) * get_tx_buff_size))
		{// increase speed(clock)
			data_to_copy += (num_of_bytes_per_sample_all_channels);
#ifdef  DEBUG
			dbg_in_thr_overflow_cnt++;
#endif
		}
#ifdef  DEBUG
		dbg_copy_size = data_to_copy;
#endif
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
		DEV_IOCTL(cfg_hndl->usb_hw,
				IOCTL_USB_DEVICE_SENT_DATA_TO_IN_ENDPOINT, &data_to_endpoint);
		runtime_hndl->curr_read_pos_in_tx_buffer = curr_read_pos_in_tx_buffer;
	}
	else
	{
		/* Setup error, stall the device */
		DEV_IOCTL(cfg_hndl->usb_hw, IOCTL_USB_DEVICE_STALL_CONTROL_ENDPOINT);
#ifdef  DEBUG
		dbg_in_underflow_cnt++;
#endif
	}
}


void new_sample_rate_requested(struct dev_desc_t *adev)
{
	struct usb_audio_class_cfg_t *cfg_hndl;
	struct usb_audio_class_runtime_t *runtime_hndl;
	struct set_data_to_in_endpoint_t  data_to_endpoint;
	uint8_t  tx_buff[3];
	uint32_t usb_feedback_sample_rate;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(usb_audio_class, adev);
	runtime_hndl = DEV_GET_RUNTIME_DATA_POINTER(usb_audio_class, adev);

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
	DEV_IOCTL(cfg_hndl->usb_hw,
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
#ifdef  DEBUG
				dbg_out_underflow_cnt++;
#endif
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
#ifdef  DEBUG
		dbg_out_underflow_cnt++;
#endif
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
#ifdef  DEBUG
		dbg_in_overflow_cnt++;
#endif
		*buff = NULL;
		*ret_buff_size = 0;
		runtime_hndl->tx_buffer_was_supplied = 0;
		return 1;
	}


	if (1 == runtime_hndl->tx_buffer_was_supplied)
	{
#ifdef  DEBUG
		dbg_in_overflow_cnt++;
#endif
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
#ifdef  DEBUG
		dbg_in_overflow_cnt++;
#endif
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

		DEV_IOCTL(cfg_hndl->usb_hw,
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
	rx_buff = (uint8_t **)os_safe_malloc(num_of_rx_buffers * sizeof(uint8_t*));
	errors_api_check_if_malloc_succeed(rx_buff);
	runtime_hndl->rx_buff = rx_buff;

	rx_buff_status = (uint8_t *)os_safe_malloc(num_of_rx_buffers);
	errors_api_check_if_malloc_succeed(rx_buff_status);
	runtime_hndl->rx_buff_status = rx_buff_status;

	for (i = 0; i < num_of_rx_buffers; i++)
	{
		new_buff = (uint8_t*)os_safe_malloc(get_rx_buff_size);
		errors_api_check_if_malloc_succeed(new_buff);
		rx_buff[i] = new_buff;
		rx_buff_status[i] = USB_AUDIO_CLASS_BUFF_IDLE ;
	}
	runtime_hndl->host_started_playback = 0;
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
	new_buff = (uint8_t*)os_safe_malloc(max_host_in_pckt_size);
	errors_api_check_if_malloc_succeed(new_buff);
	runtime_hndl->tx_pckt_buff = new_buff;

	runtime_hndl->curr_write_pos_in_tx_buffer = 0;
	runtime_hndl->curr_read_pos_in_tx_buffer = 0;

	// init tx_buff last, as it serves for detecting if host_in is initialized
	new_buff = (uint8_t*)os_safe_malloc(
					cfg_hndl->get_tx_buff_size * NUM_OF_TX_BUFFERS);
	errors_api_check_if_malloc_succeed(new_buff);
	runtime_hndl->tx_buff = new_buff;
	runtime_hndl->host_started_recording = 0;
}


static void init_default_volumes_and_mutes(
			struct usb_audio_class_runtime_t *runtime_hndl)
{
	uint16_t i;
	int16_t *volume_arr;

	volume_arr = runtime_hndl->curr_playback_volume;
	for (i = 0; i < NUM_OF_PLAYBACK_CHANNELS; i++)
	{
		volume_arr[i] = 0x000; // 0db
	}
	volume_arr = runtime_hndl->curr_recording_volume;
	for (i = 0; i < NUM_OF_RECORDING_CHANNELS; i++)
	{
		volume_arr[i] = 0x000; // 0db
	}
	runtime_hndl->recording_mute = 0;
	runtime_hndl->playback_mute = 0;
}


static void start_audio_class(struct dev_desc_t *adev,
		struct usb_audio_class_cfg_t *cfg_hndl,
		struct usb_audio_class_runtime_t *runtime_hndl)
{
#ifdef DEBUG
	dbg_usb_audio_class_runtime_hndl = runtime_hndl;
#endif

	init_default_volumes_and_mutes(runtime_hndl);
	runtime_hndl->request_state = REQ_STATE_IDLE;
	runtime_hndl->app_is_getting_data = 0;

	// map to int16_t with 1/256db step : range = [0x8001, 0x7fff]
	runtime_hndl->max_recording_volume =
			(int16_t)(cfg_hndl->max_recording_volume_db * 256);
	runtime_hndl->min_recording_volume =
			(int16_t)(cfg_hndl->min_recording_volume_db * 256);
	runtime_hndl->recording_volume_res =
			(int16_t)(cfg_hndl->recording_volume_resolution_db * 256);
	runtime_hndl->max_playback_volume =
			(int16_t)(cfg_hndl->max_playback_volume_db * 256);
	runtime_hndl->min_playback_volume =
			(int16_t)(cfg_hndl->min_playback_volume_db * 256);
	runtime_hndl->playback_volume_res =
			(int16_t)(cfg_hndl->playback_volume_resolution_db * 256);

	runtime_hndl->playback_volumes_changed = 0;
	runtime_hndl->playback_mute_changed = 0;
	runtime_hndl->recording_volumes_changed = 0;
	runtime_hndl->recording_mute_changed = 0;

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
		DEV_IOCTL(cfg_hndl->usb_hw,
				IOCTL_USB_DEVICE_SENT_DATA_TO_IN_ENDPOINT, &data_to_endpoint);
	}
}


static uint8_t get_master_volumes(
		struct usb_audio_class_runtime_t *runtime_hndl,
		struct usb_audio_class_get_volumes_t  *get_volumes)
{
	runtime_hndl->app_is_getting_data = 1;
	// from this point new USB out request for volume change will get STALL
	// till app_is_getting_data go to 0
	if (REQ_STATE_IDLE != runtime_hndl->request_state)
	{
		runtime_hndl->app_is_getting_data = 0;
		return 1;
	}
	get_volumes->playback_volumes_changed =
				runtime_hndl->playback_volumes_changed;
	get_volumes->recording_volumes_changed =
			runtime_hndl->recording_volumes_changed;
	// according to UAC specification, the volume value is from -128db to +128db
	// with resolution of volumes is 1/256db. so /256 to convert to float
	get_volumes->curr_playback_volume_db =
			((float)runtime_hndl->curr_playback_volume[0]) / 256;
	get_volumes->curr_recording_volume_db =
			((float)runtime_hndl->curr_recording_volume[0]) / 256;
	runtime_hndl->playback_volumes_changed = 0;
	runtime_hndl->recording_volumes_changed = 0;
	runtime_hndl->app_is_getting_data = 0;
	return 0;
}


static uint8_t get_master_mutes(
		struct usb_audio_class_runtime_t *runtime_hndl,
		struct usb_audio_class_get_mutes_t *get_mutes)
{
	runtime_hndl->app_is_getting_data = 1;
	// from this point new USB out request for mute change will get STALL
	// till app_is_getting_data go to 0
	if (REQ_STATE_IDLE != runtime_hndl->request_state)
	{
		runtime_hndl->app_is_getting_data = 0;
		return 1;
	}
	get_mutes->playback_mute_changed = runtime_hndl->playback_mute_changed;
	get_mutes->recording_mute_changed = runtime_hndl->recording_mute_changed;
	get_mutes->curr_playback_mute = runtime_hndl->playback_mute;
	get_mutes->curr_recording_mute = runtime_hndl->recording_mute;
	runtime_hndl->playback_mute_changed = 0;
	runtime_hndl->recording_mute_changed = 0;
	runtime_hndl->app_is_getting_data = 0;
	return 0;
}


/**
 * usb_audio_class_ioctl()
 *
 * return:
 */
static uint8_t usb_audio_class_ioctl( struct dev_desc_t *adev,
		uint8_t aIoctl_num, void * aIoctl_param1, void * aIoctl_param2)
{
	struct usb_audio_class_cfg_t *cfg_hndl;
	struct usb_audio_class_runtime_t *runtime_hndl;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(usb_audio_class, adev);
	runtime_hndl = DEV_GET_RUNTIME_DATA_POINTER(usb_audio_class, adev);

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
	case USB_AUDIO_CLASS_IOCTL_GET_MASTER_VOLUMES :
		return get_master_volumes(runtime_hndl, aIoctl_param1);
		break;
	case USB_AUDIO_CLASS_IOCTL_GET_MASTER_MUTES :
		return get_master_mutes(runtime_hndl, aIoctl_param1);
		break;
	case USB_AUDIO_CLASS_IOCTL_GET_IF_HOST_RECORDING:
		*(uint8_t *) aIoctl_param1 = runtime_hndl->host_started_recording;
		break;
	case USB_AUDIO_CLASS_IOCTL_GET_IF_HOST_IS_STREAMING_OUT:
		*(uint8_t *) aIoctl_param1 = runtime_hndl->host_started_playback;
		break;
	default :
		return 1;
	}
	return 0;
}

#define MODULE_NAME                  usb_audio_class
#define MODULE_IOCTL_FUNCTION        usb_audio_class_ioctl
#include "add_module.h"
