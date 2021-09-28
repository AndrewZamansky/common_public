/*
 *
 * file :   NAU83GXX.c
 *
 *
 */

#include "_project.h"
#include "dev_management_api.h"

//#define DEBUG
#include "PRINTF_api.h"

#include "os_wrapper.h"
#include "NAU83GXX_api.h"
#include "NAU83GXX.h"
#include "i2c_api.h"
#include "gpio_api.h"


#ifndef  NAU83GXX_TASK_PRIORITY
	#error  "NAU83GXX_TASK_PRIORITY should be define in project"
#endif
#ifndef  NAU83GXX_TASK_STACK_SIZE
	#error  "NAU83GXX_TASK_STACK_SIZE should be define in project"
#endif

#define I2C_DEV_DESC 0x46

#define KCS_I2C_DSP_MAX_TX_SIZE            ( 96 )

//#define VBST_LEVEL 0x3402
#define AUTO_START_BIQUAD


enum init_sel_e {
	INIT_WITH_RESET_ONLY,
	INIT_WITH_REG_AND_KCS
};

enum main_msg_e {
	MSG_TYPE_INIT_HW,
	MSG_TYPE_REINIT_I2C_REGS,
	MSG_KCS_I2C_SIMPLE_CMD,
	MSG_KCS_I2C_GET_CMD,
	MSG_KCS_EXIT_GET_STATE,
	MSG_START_COLLECT_DATA_FOR_SEND,
	MSG_ADD_DATA_FOR_SEND,
	MSG_SEND_COLLECTED_SETUP,
	MSG_SEND_SETUP,
	MSG_CONTINUE_RECOVERY,
	MSG_POWER_DOWN,
};


struct kcs_simple_cmd_msg_t {
	uint8_t   cmd;
	uint32_t  *p_recieved_U32_reply;
};

struct kcs_get_cmd_msg_t {
	uint16_t  offset;
	uint16_t  size_to_read;
	uint16_t  *recieved_size;
	uint8_t    **recieved_data;
	uint8_t    cmd;
};

struct init_hw_msg_t {
	const struct S_I2CCMD *biquad_init_arr;
	uint16_t biquad_init_arr_size;
	uint8_t *kcs_spkr_param_data;
	size_t kcs_spkr_param_size;
	end_of_ioctl_callback_f  end_of_ioctl_callback;
	uint8_t init_sel;
};


struct start_collect_data_for_send_msg_t {
	uint16_t        offset;
	uint16_t        size_to_collect;
};


struct add_data_for_send_msg_t {
	uint8_t const        *data;
	uint16_t        size;
};


struct send_setup_data_msg_t {
	uint8_t         *data;
	uint16_t        size;
	uint16_t        offset;
};


struct task_message_t
{
	uint8_t    msg_type;
	union {
		struct init_hw_msg_t  init_hw_msg;
		struct kcs_simple_cmd_msg_t  kcs_simple_cmd_msg;
		struct kcs_get_cmd_msg_t kcs_get_cmd_msg;
		struct start_collect_data_for_send_msg_t
						start_collect_data_for_send_msg;
		struct add_data_for_send_msg_t add_data_for_send_msg;
		struct send_setup_data_msg_t send_setup_data_msg;
	};
};

struct reply_message_t {
	uint8_t rc;
};

enum state_e {
	STATE_NOT_INITIALIZED = 0,
	STATE_DRIVER_INIT_DONE,
	STATE_IDLE,
	STATE_PROCESSING_GET_CMD,
	STATE_COLLECTING_DATA_FOR_SEND
};

extern uint8_t nau83gxx_init_i2c_regs(struct NAU83GXX_config_t *config_handle);
extern uint8_t nau83gxx_write(struct dev_desc_t *i2c_dev, uint8_t device_addr,
		uint16_t reg_addr, const uint8_t *data, size_t write_data_size);
extern uint8_t nau83gxx_read(struct dev_desc_t *i2c_dev, uint8_t device_addr,
		uint16_t reg_addr, uint8_t *read_data, uint16_t size_to_read);
extern uint8_t nau83gxx_init_biquad(const struct S_I2CCMD *cmd_biquad_init,
		uint16_t cmd_biquad_init_size, long int device_addr,
		struct dev_desc_t *i2c_dev);
extern uint8_t nau83gxx_read_wordU16(struct dev_desc_t *i2c_dev,
		uint8_t device_addr, uint16_t reg_addr, uint16_t *read_dat);
extern uint8_t nau83gxx_write_wordU16(struct dev_desc_t *i2c_dev,
		uint8_t device_addr, uint16_t reg_addr, uint16_t write_dat);
extern void NAU83GXX_start_recovery_sequence(
			struct NAU83GXX_config_t *config_handle);
extern void NAU83GXX_OCP_recovery(struct NAU83GXX_config_t *config_handle);


static uint8_t send_simple_cmd(
	struct NAU83GXX_config_t *config_handle, uint8_t cmd, uint32_t *response);
static uint8_t patch_for_correct_setup_write(
				struct NAU83GXX_config_t *config_handle);
static uint8_t send_header_and_update_lenU32(
		struct NAU83GXX_config_t *config_handle, uint8_t cmd,
		uint16_t sendLen, uint16_t *msg_len_u32, uint16_t *padding);


/**
 * NAU83GXX_callback()
 *
 * return:
 */
static uint8_t NAU83GXX_callback(struct dev_desc_t *adev,
		const uint8_t aCallback_num,
		 void * aCallback_param1, void * aCallback_param2)
{
	struct NAU83GXX_config_t *config_handle;
	struct NAU83GXX_runtime_t *runtime_handle;
	uint8_t irq_not_in_idle;
	struct task_message_t msg;
	os_queue_t msg_queue;


	config_handle = DEV_GET_CONFIG_DATA_POINTER(NAU83GXX, adev);
	runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(NAU83GXX, adev);
	msg_queue = runtime_handle->msg_queue;
	if (NULL == msg_queue) return 1;  // driver not ready to receive messages

	if (CALLBACK_INTERRUPT_ARRIVED == aCallback_num)
	{
		DEV_IOCTL_1_PARAMS(config_handle->irq_pin,
				IOCTL_GPIO_TEST_IF_PINS_NOT_IN_IDLE, &irq_not_in_idle);
		if (irq_not_in_idle)
		{
			msg.msg_type = MSG_CONTINUE_RECOVERY;
			if (OS_QUEUE_SEND_SUCCESS !=
								os_queue_send_without_wait(msg_queue, &msg))
			{
				// the queue is full, so run recovery at the end of next message
				runtime_handle->do_recovery = 0;
			}
		}
	}
	return 0;
}


static uint8_t kcs_i2c_write(struct NAU83GXX_config_t *config_handle,
					uint8_t const *write_data, size_t write_data_size)
{
	return nau83gxx_write(config_handle->i2c_dev, config_handle->dev_addr,
			NAU83GXX_DSP_REG, write_data, write_data_size);

}

static uint8_t kcs_i2c_read(
		struct NAU83GXX_config_t *config_handle, uint8_t *read_data)
{
	uint8_t rc;

	rc = nau83gxx_read(config_handle->i2c_dev, config_handle->dev_addr,
			NAU83GXX_DSP_REG, read_data, SIZE_OF_STD_DSP_WORD);

	return rc;
}


static uint8_t check_if_idle(struct NAU83GXX_config_t *config_handle)
{
	int retries;
	uint8_t rc;
	uint32_t u32Data;

	for (retries = 10; retries > 0; retries--)
	{
		rc = kcs_i2c_read(config_handle, (uint8_t *)&u32Data);
		if(rc)
		{
			// communication error
			PRINTF_DBG("IDLE_WORD RC_COMMUNICATION_ERROR\n\r");
		}
		if(u32Data == DSP_COMM_IDLE_WORD)
		{
			break;
		}
	}
	// we could not establish a connection to DSP - maybe it is not clocked,
	//or previous synchronization issue
	if(retries == 0)
	{
		PRINTF_DBG("RC_TIMEDOUT_FOR_IDLE_WORD\n\r");
		return RC_TIMEDOUT_FOR_IDLE_WORD;
	}
	return RC_OK;
}


static uint8_t get_preamble(struct NAU83GXX_config_t *config_handle,
		uint16_t *lenInU32)
{
	int retries;
	uint8_t replyStatus = 0;
	uint8_t rc = RC_OK;
	uint8_t data[SIZE_OF_STD_DSP_WORD] = {0};

	for (retries = 10; retries > 0; retries--)
	{
		rc = kcs_i2c_read(config_handle, data);
		if (rc)
		{
			// communication error
			PRINTF_DBG("PREAMBLE RC_COMMUNICATION_ERROR\n\r");
		}
		// check for preamble;
		if(data[0] == ((DSP_COMM_PREAMBLE >> 8) & 0xFF) &&
			data[1] == ((DSP_COMM_PREAMBLE) & 0xFF))
		{
			*lenInU32 = data[2] & 0x03;
			*lenInU32 +=  (data[3] << 2);
			replyStatus = data[2] >> 2;
			break;
		}
	}

	// DSP data was not ready after the amount of retries
	if(retries == 0)
	{
		PRINTF_DBG("RC_PREAMBLE_NOT_FOUND\n\r");
		return RC_PREAMBLE_NOT_FOUND;
	}

	// Check they reply from the DSP
	if(replyStatus != 0) return (RC_REPLY_STATUS_ERR | replyStatus);
	else return rc;
}


static uint8_t send_header_and_update_lenU32(
		struct NAU83GXX_config_t *config_handle,
		uint8_t cmd, uint16_t sendLen, uint16_t *msg_len_u32, uint16_t *padding)
{
	uint8_t rc;
	uint16_t lenInU32;
	uint8_t data[4];

	//get idle phrase
	rc = check_if_idle(config_handle);
	if(rc) return rc;

	// build the data to be transfered
	lenInU32 = (sendLen + 3) / 4;
	*padding = (lenInU32 * 4) - sendLen;

	// if data is more than one fragment, add a postamble
	if(lenInU32 != 0) lenInU32++;

	data[0] = (DSP_COMM_PREAMBLE >> 8) & 0xFF;
	data[1] = (DSP_COMM_PREAMBLE) & 0xFF;
	data[2] =  (cmd & 0x3f) << 2;
	data[2] |= (lenInU32 & 0x3);
	data[3] = (lenInU32 >> 2);

	rc = kcs_i2c_write(config_handle, data, SIZE_OF_STD_DSP_WORD);
	if(rc)
	{
		// communication error
		PRINTF_DBG("RC_COMMUNICATION_ERROR preamble\n\r");
		return RC_COMMUNICATION_ERROR;
	}
	*msg_len_u32 = lenInU32;
	return RC_OK;
}


static uint8_t send_offset_and_length(struct NAU83GXX_config_t *config_handle,
							uint16_t data_len, uint16_t addr_offset)
{
	uint8_t rc;
	uint8_t data[SIZE_OF_STD_DSP_WORD] = {0};

	//send cmd parameters
	if(0 == data_len)
	{
		PRINTF_DBG("OFFSET AND LENGTH RC_INTERNAL_ERROR\n\r");
		return RC_INTERNAL_ERROR;
	}
	data[0] = addr_offset & 0xFF;
	data[1] = (addr_offset >> 8) & 0xFF;
	data[2] = data_len & 0xFF;
	data[3] = (data_len >> 8) & 0xFF;

	// send fragment
	rc = kcs_i2c_write(config_handle, data, 4);
	if (rc)
	{
		// communication error
		return RC_COMMUNICATION_ERROR;
	}
	return rc;
}


static uint8_t send_payload(struct NAU83GXX_config_t *config_handle,
				const uint8_t *dataSend, uint16_t sendLen)
{
	int i,j, data_size;
	uint8_t rc;
	uint16_t lenInU32;
	uint16_t left_size;
	uint8_t data[SIZE_OF_STD_DSP_WORD] = {0};

	if (0 == sendLen) return RC_OK;

	lenInU32 = (sendLen + 3) / 4;
	left_size = sendLen;
	// send payload
	for (i = 0; i < lenInU32 ; ++i)
	{
		// initialize output data
		memset(data, 0, SIZE_OF_STD_DSP_WORD);

		data_size = left_size;
		if(left_size > 4) data_size = 4;

		// build next fragment
		for (j = 0; j < data_size; ++j)
		{
			data[j] = dataSend[(i * 4) + j];
		}

		// send fragment
		rc = kcs_i2c_write(config_handle, data, data_size);
		if(rc)
		{
			// communication error
			PRINTF_DBG("PAYLOAD RC_COMMUNICATION_ERROR fragment %d\n\r",i);
			return RC_COMMUNICATION_ERROR;
		}
		left_size -= 4;
	}
	return RC_OK;
}


static uint8_t get_reply(struct NAU83GXX_config_t *config_handle,
		uint32_t lenInU32, uint16_t reqRecvLen, uint8_t *dataRecv,
		uint16_t *recvLen)
{
	int i, j;
	int32_t verifyLen, padding;
	uint8_t data[SIZE_OF_STD_DSP_WORD] = {0};
	uint8_t rc;
	// read all data except two last word
	if(NULL == dataRecv)
	{
		return RC_UNEXPECTED_REPLY;
	}

	*recvLen = 0;

	for (i = 0;  i < lenInU32 - 1; ++i)
	{
		// get fragment
		rc = kcs_i2c_read(config_handle, data);
		if(rc)
		{
			// communication error
			PRINTF_DBG("REPLY RC_COMMUNICATION_ERROR\n\r");
		}
		for (j = 0; j < SIZE_OF_STD_DSP_WORD; ++j)
		{
			if( (i*4 + j) < reqRecvLen)
				dataRecv[ (i * 4) + j ] =  data[j];
		}
	}

	//read last word (postamble)
	rc = kcs_i2c_read(config_handle, data);
	if(rc)
	{
		// communication error
		PRINTF_DBG("TRAILING FRAGMENT RC_COMMUNICATION_ERROR\n\r");
	}

	verifyLen = data[0] + ((data[1] & 0xc0) << 2);
	if(verifyLen != lenInU32)
	{
		PRINTF_DBG("ERROR LEN_POST = %02X, expected %02X\n\r",
				verifyLen,lenInU32);
		return RC_REPLY_DATA_INTEGRITY_ERROR;
	}
	padding =  (data[1] & 0x30) >> 4;
	*recvLen = (lenInU32 - 1) * 4 - padding;

	return RC_OK;
}


static uint8_t send_postamble_and_get_reply(
			struct NAU83GXX_config_t *config_handle,
			uint16_t lenInU32, uint16_t padding,
			uint16_t reqRecvLen, uint8_t *dataRecv, uint16_t *recvLen)
{
	uint8_t rc;
	uint8_t data[SIZE_OF_STD_DSP_WORD];

	if(lenInU32 > 0)
	{
		data[0] =  (lenInU32 & 0xff);
		data[1] = (lenInU32 & 0x300) >> 2;
		data[1] |= (padding & 0x03) << 4;
		data[2] = data[3] = 0;

		// send fragment
		rc = kcs_i2c_write(config_handle, data, SIZE_OF_STD_DSP_WORD);
		if(rc)
		{
			// communication error
			PRINTF_DBG("RC_COMMUNICATION_ERROR postamble\n\r");
			return RC_COMMUNICATION_ERROR;
		}
	}

	// at this stage, command was send, now we will get the reply

	//get preamble
	rc = get_preamble(config_handle, &lenInU32);
	if(rc) return rc;

	// if reply has data - handle data
	if(lenInU32 > 0)
	{
		rc = get_reply(config_handle, lenInU32, reqRecvLen, dataRecv, recvLen);
		if(rc) return rc;
		if( *recvLen > reqRecvLen) return RC_REPLY_LEN_TOO_LONG;
	}
	return RC_OK;
}


static uint8_t patch_for_correct_setup_write(
							struct NAU83GXX_config_t *config_handle)
{
	uint16_t lenInU32, padding;
	uint8_t tmp_rx_buf[4];
	uint16_t tmp_rx_len;
	uint8_t rc;

	rc = send_header_and_update_lenU32(
			config_handle, DSP_CMD_GET_KCS_RESULTS,
			4, &lenInU32, &padding);
	if(rc) return rc;

	rc = send_offset_and_length(config_handle, 4, 0);
	if(rc) return rc;

	return send_postamble_and_get_reply(
			config_handle, lenInU32, padding, 4, tmp_rx_buf, &tmp_rx_len);
}


static uint8_t send_kcs_setup(
		struct NAU83GXX_config_t *config_handle, uint16_t  addr_offset,
		const uint8_t *p_send_buffer, uint16_t  size_to_send)
{
	uint16_t lenInU32, padding;
	uint16_t  retries;
	uint8_t rc = RC_OK;
	uint16_t  send_len;

	if (KCS_I2C_MAX_BUFFER_IN_BYTES <= size_to_send)
	{
		return RC_INPUT_SIZE_TOO_LARGE_ERR;
	}

	while (size_to_send)
	{
		if (KCS_I2C_DSP_MAX_TX_SIZE > size_to_send)
		{
			send_len = size_to_send;
		}
		else
		{
			send_len = KCS_I2C_DSP_MAX_TX_SIZE;
		}

		retries = 3;
		rc = 1;
		while (rc && (retries--))
		{
			rc = send_header_and_update_lenU32(config_handle,
					DSP_CMD_SET_KCS_SETUP, send_len + 4, &lenInU32, &padding);
			if(rc) continue;

			rc = send_offset_and_length(config_handle, send_len, addr_offset);
			if(rc) continue;

			rc = send_payload(config_handle, p_send_buffer, send_len);
			if(rc) continue;

			rc = send_postamble_and_get_reply(
					config_handle, lenInU32, padding, 4, NULL, NULL);
			if(rc) continue;

		}
		if (rc) return rc; //if after repeats still error - exit
		//maybe needed after each write
		//	patch_for_correct_setup_write(config_handle);

		p_send_buffer += send_len;
		size_to_send -= send_len;
		addr_offset += send_len;
	}
	while (RC_OK != patch_for_correct_setup_write(config_handle));
	return RC_OK;
}


static uint8_t process_init_hw_msg(struct NAU83GXX_config_t *config_handle,
		struct NAU83GXX_runtime_t *runtime_handle,
		struct init_hw_msg_t  *p_init_hw_msg)
{
	uint16_t read_dat;
	uint8_t dev_addr;
	struct dev_desc_t * i2c_dev;
	uint32_t status;
	uint8_t rc;

	dev_addr = config_handle->dev_addr;
	i2c_dev = config_handle->i2c_dev;

	rc = RC_OK;
	switch(p_init_hw_msg->init_sel)
	{
	case INIT_WITH_RESET_ONLY:
		rc = nau83gxx_write_wordU16(i2c_dev, dev_addr, 0x0, 0x0);
		if (rc) return rc;
		break;
	case INIT_WITH_REG_AND_KCS:
		nau83gxx_read_wordU16(i2c_dev, dev_addr, I2C_DEV_DESC, &read_dat);
		read_dat = read_dat & 0xF0;
		if (read_dat != config_handle->chip_type)
		{
			CRITICAL_ERROR("wrong chip type");
		}

		rc = nau83gxx_init_i2c_regs(config_handle);
		if (rc) return rc;
		//os_delay_ms(500);
		while (RC_OK != send_simple_cmd(config_handle, DSP_CMD_GET_STATUS,
																&status))
		{
			os_delay_ms(5);
		}

		#ifdef VBST_LEVEL
		//changes VBST to defined VBST_LEVEL
			nau83gxx_write_wordU16(i2c_dev, dev_addr, 0x17, VBST_LEVEL);
		#endif
		#ifdef AUTO_START_BIQUAD
			nau83gxx_init_biquad(p_init_hw_msg->biquad_init_arr,
				p_init_hw_msg->biquad_init_arr_size, dev_addr, i2c_dev);
		#endif

		rc = send_kcs_setup(config_handle, 0,
				p_init_hw_msg->kcs_spkr_param_data,
				p_init_hw_msg->kcs_spkr_param_size);
		if (NULL != p_init_hw_msg->end_of_ioctl_callback)
		{
			p_init_hw_msg->end_of_ioctl_callback(
							IOCTL_HW_IS_READY_TO_INIT_WITH_FULL_KCS);
		}
		break;
	default:
		break;
	}

	if (rc)
	{
		CRITICAL_ERROR("cannot init NAU83GXX");
	}
	runtime_handle->state = STATE_IDLE;
	return rc;
}


static uint8_t process_power_down_msg(
		struct NAU83GXX_config_t *config_handle,
		struct NAU83GXX_runtime_t *runtime_handle)
{
	uint8_t rc;

	DEV_IOCTL_0_PARAMS(config_handle->irq_pin, IOCTL_DEVICE_STOP);
	rc = nau83gxx_write_wordU16(
			config_handle->i2c_dev, config_handle->dev_addr, 0x0, 0x0);
	runtime_handle->state = STATE_NOT_INITIALIZED;
	free(runtime_handle->dataBuf);
	return rc;
}

static uint8_t process_reinit_i2c_regs_msg(
			struct NAU83GXX_config_t *config_handle)
{
	return nau83gxx_init_i2c_regs(config_handle);
}


static uint8_t send_simple_cmd(
		struct NAU83GXX_config_t *config_handle,
		uint8_t cmd, uint32_t *response)
{
	uint16_t lenInU32, padding;
	uint16_t recieved_len;
	uint8_t response_data[4];
	uint8_t rc;

	rc = send_header_and_update_lenU32(
			config_handle, cmd, 0, &lenInU32, &padding);
	if(rc) return rc;
	rc = send_postamble_and_get_reply(config_handle, lenInU32, padding,
			4, response_data, &recieved_len);
	if(rc) return rc;

	*response = (response_data[0]) | (response_data[1] << 8) |
			(response_data[2] << 16) | (response_data[3] << 24);
	return RC_OK;
}


static uint8_t process_kcs_simple_cmd_msg(
		struct NAU83GXX_config_t *config_handle,
		struct NAU83GXX_runtime_t *runtime_handle,
		struct kcs_simple_cmd_msg_t  *p_kcs_simple_cmd_msg)
{
	if (STATE_IDLE != runtime_handle->state)
	{
		return RC_DRIVER_BUSY | runtime_handle->state;
	}

	return send_simple_cmd(config_handle,  p_kcs_simple_cmd_msg->cmd,
			 p_kcs_simple_cmd_msg->p_recieved_U32_reply);
}


static uint8_t process_kcs_get_cmd_msg(struct NAU83GXX_config_t *config_handle,
		struct NAU83GXX_runtime_t *runtime_handle,
		struct kcs_get_cmd_msg_t  *p_kcs_get_cmd_msg)
{
	uint16_t lenInU32, padding;
	uint16_t recieved_len;
	uint8_t rc;
	uint16_t size_to_read;

	if (STATE_IDLE != runtime_handle->state)
	{
		return RC_DRIVER_BUSY | runtime_handle->state;
	}
	runtime_handle->state = STATE_PROCESSING_GET_CMD;

	rc = send_header_and_update_lenU32(
			config_handle, p_kcs_get_cmd_msg->cmd, 4, &lenInU32, &padding);
	if(rc) return rc;

	rc = send_offset_and_length(config_handle,
			p_kcs_get_cmd_msg->size_to_read, p_kcs_get_cmd_msg->offset);
	if(rc) return rc;

	size_to_read = p_kcs_get_cmd_msg->size_to_read;
	rc = send_postamble_and_get_reply(config_handle, lenInU32, padding,
			size_to_read, runtime_handle->dataBuf, &recieved_len);
	*p_kcs_get_cmd_msg->recieved_size = recieved_len;
	*p_kcs_get_cmd_msg->recieved_data = runtime_handle->dataBuf;
	if(rc) return rc;

	return RC_OK;
}


static uint8_t process_kcs_exit_get_state_msg(
		struct NAU83GXX_runtime_t *runtime_handle)
{
	if (STATE_PROCESSING_GET_CMD != runtime_handle->state)
	{
		return RC_DRIVER_BUSY | runtime_handle->state;
	}
	runtime_handle->state = STATE_IDLE;
	return RC_OK;
}



static uint8_t process_start_collect_data_for_send_msg(
		struct NAU83GXX_config_t *config_handle,
		struct NAU83GXX_runtime_t *runtime_handle,
		struct start_collect_data_for_send_msg_t  *start_collect_data_msg)
{
	if (STATE_IDLE != runtime_handle->state)
	{
		return RC_DRIVER_BUSY | runtime_handle->state;
	}
	if (KCS_I2C_MAX_BUFFER_IN_BYTES < start_collect_data_msg->size_to_collect)
	{
		return RC_INPUT_SIZE_TOO_LARGE_ERR;
	}
	runtime_handle->state = STATE_COLLECTING_DATA_FOR_SEND;
	runtime_handle->addr_offset = start_collect_data_msg->offset;
	runtime_handle->sendLen = start_collect_data_msg->size_to_collect;
	runtime_handle->dataSetBufPos = 0;
	return RC_OK;
}



static uint8_t process_add_data_for_send_msg(
		struct NAU83GXX_config_t *config_handle,
		struct NAU83GXX_runtime_t *runtime_handle,
		struct add_data_for_send_msg_t  *add_data_msg)
{
	uint32_t dataSetBufPos;
	uint32_t dataSetBufPos_u32; // u32 to avoid overflow of u16
	uint16_t new_data_size;

	if (STATE_COLLECTING_DATA_FOR_SEND != runtime_handle->state)
	{
		return RC_DRIVER_BUSY | runtime_handle->state;
	}

	new_data_size = add_data_msg->size;
	dataSetBufPos = runtime_handle->dataSetBufPos;
	dataSetBufPos_u32 = dataSetBufPos + new_data_size;
	if (runtime_handle->sendLen < dataSetBufPos_u32)
	{
		return RC_BUFFER_OVERFLOW_ERR;
	}
	memcpy(&runtime_handle->dataBuf[dataSetBufPos],
						add_data_msg->data, new_data_size);
	runtime_handle->dataSetBufPos = dataSetBufPos_u32;
	return RC_OK;
}


static uint8_t process_kcs_send_collected_setup_data_msg(
		struct NAU83GXX_config_t *config_handle,
		struct NAU83GXX_runtime_t *runtime_handle)
{
	if (STATE_COLLECTING_DATA_FOR_SEND != runtime_handle->state)
	{
		return RC_DRIVER_BUSY | runtime_handle->state;
	}
	if (runtime_handle->dataSetBufPos != runtime_handle->sendLen)
	{
		return RC_BUFFER_NOT_EQUAL_TO_SIZE;
	}
	runtime_handle->state = STATE_IDLE;

	return send_kcs_setup(config_handle, runtime_handle->addr_offset,
			runtime_handle->dataBuf, runtime_handle->sendLen);
}


static uint8_t process_kcs_send_setup_data_msg(
		struct NAU83GXX_config_t *config_handle,
		struct NAU83GXX_runtime_t *runtime_handle,
		struct send_setup_data_msg_t *p_send_setup_data_msg)
{
	if (STATE_IDLE != runtime_handle->state)
	{
		return RC_DRIVER_BUSY | runtime_handle->state;
	}

	return send_kcs_setup(config_handle, p_send_setup_data_msg->offset,
			p_send_setup_data_msg->data, p_send_setup_data_msg->size);
}


/**
 * nau83gxx_task()
 *
 * return:
 */
static void nau83gxx_task (void * adev)
{
	os_queue_t msg_queue;
	os_queue_t wait_for_finish_queue;
	struct task_message_t msg;
	struct reply_message_t release_wait_msg;
	struct NAU83GXX_config_t *config_handle;
	struct NAU83GXX_runtime_t *runtime_handle;
	uint8_t rc = RC_OK;

	config_handle = DEV_GET_CONFIG_DATA_POINTER(NAU83GXX, adev);
	runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(NAU83GXX, adev);

	msg_queue = os_create_queue(1, sizeof(struct task_message_t));
	if (NULL == msg_queue)
	{
		CRITICAL_ERROR("i2c queue failed to create");
	}
	runtime_handle->msg_queue = msg_queue;

	wait_for_finish_queue = os_create_queue(1, sizeof(struct reply_message_t));
	if (NULL == wait_for_finish_queue)
	{
		CRITICAL_ERROR("wait_queue failed to create");
	}
	runtime_handle->wait_for_finish_queue = wait_for_finish_queue;

	runtime_handle->dataBuf = (uint8_t *)malloc(
			KCS_I2C_MAX_BUFFER_IN_BYTES * (sizeof(uint8_t)));

	while (1)
	{
		if( OS_QUEUE_RECEIVE_SUCCESS !=
				os_queue_receive_infinite_wait( msg_queue , &msg))
		{
			continue;
		}
		if (STATE_NOT_INITIALIZED == runtime_handle->state) continue;

		switch(msg.msg_type)
		{
		case MSG_TYPE_INIT_HW:
			rc = process_init_hw_msg(
					config_handle, runtime_handle, &msg.init_hw_msg);
			break;
		case MSG_TYPE_REINIT_I2C_REGS:
			rc = process_reinit_i2c_regs_msg(config_handle);
			break;
		case MSG_KCS_I2C_SIMPLE_CMD:
			rc = process_kcs_simple_cmd_msg(
					config_handle, runtime_handle, &msg.kcs_simple_cmd_msg);
			break;
		case MSG_KCS_I2C_GET_CMD:
			rc = process_kcs_get_cmd_msg(
					config_handle, runtime_handle, &msg.kcs_get_cmd_msg);
			break;
		case MSG_KCS_EXIT_GET_STATE:
			rc = process_kcs_exit_get_state_msg(runtime_handle);
			break;
		case MSG_START_COLLECT_DATA_FOR_SEND:
			rc = process_start_collect_data_for_send_msg(config_handle,
					runtime_handle, &msg.start_collect_data_for_send_msg);
			break;
		case MSG_ADD_DATA_FOR_SEND:
			rc = process_add_data_for_send_msg(config_handle,
					runtime_handle, &msg.add_data_for_send_msg);
			break;
		case MSG_SEND_COLLECTED_SETUP:
			rc = process_kcs_send_collected_setup_data_msg(
									config_handle, runtime_handle);
			break;
		case MSG_SEND_SETUP:
			rc = process_kcs_send_setup_data_msg(config_handle,
							runtime_handle, &msg.send_setup_data_msg);
			break;
		case MSG_CONTINUE_RECOVERY:
			runtime_handle->do_recovery = 1;
			break;
		case MSG_POWER_DOWN:
			rc = process_power_down_msg(config_handle, runtime_handle);
			break;
		default:
			CRITICAL_ERROR("no such case");
			break;
		}

		if (runtime_handle->do_recovery)
		{
			NAU83GXX_OCP_recovery(config_handle);
		}
		else
		{
			release_wait_msg.rc = rc;
			os_queue_send_without_wait(
					wait_for_finish_queue, (void *) &release_wait_msg);
		}
		os_stack_test();
	}
}


static uint8_t send_msg_and_wait(struct NAU83GXX_runtime_t *runtime_handle,
		struct task_message_t *msg)
{
	struct reply_message_t release_wait_msg;
	os_queue_t msg_queue;
	os_queue_t wait_for_finish_queue;
	os_mutex_t main_mutex;

	main_mutex = runtime_handle->main_mutex;
	os_mutex_take_infinite_wait(main_mutex);

	msg_queue = runtime_handle->msg_queue;
	wait_for_finish_queue = runtime_handle->wait_for_finish_queue;
	if ((NULL == msg_queue) || (NULL == wait_for_finish_queue))
	{
		os_mutex_give(main_mutex);
		return RC_DRIVER_NOT_READY | runtime_handle->state;
	}

	// clean wait queue
	os_queue_receive_with_timeout(wait_for_finish_queue, &release_wait_msg, 0);

	os_queue_send_infinite_wait(msg_queue, msg);
	if( OS_QUEUE_RECEIVE_SUCCESS != os_queue_receive_infinite_wait(
					wait_for_finish_queue, &release_wait_msg))
	{
		os_mutex_give(main_mutex);
		return RC_DRIVER_BUSY | runtime_handle->state;
	}
	os_mutex_give(main_mutex);
	return release_wait_msg.rc;
}


static uint8_t send_msg_no_wait(struct NAU83GXX_runtime_t *runtime_handle,
		struct task_message_t *msg)
{
	os_queue_t msg_queue;
	os_mutex_t main_mutex;
	uint8_t mutex_take_ret;

	msg_queue = runtime_handle->msg_queue;
	main_mutex = runtime_handle->main_mutex;
	mutex_take_ret = os_mutex_take_with_timeout(main_mutex, 0);
	if (OS_MUTEX_TAKE_SUCCESS != mutex_take_ret)
	{
		return RC_DRIVER_BUSY | runtime_handle->state;
	}

	if (NULL == msg_queue)
	{
		os_mutex_give(main_mutex);
		return RC_DRIVER_NOT_READY | runtime_handle->state;
	}

	if (OS_QUEUE_SEND_SUCCESS != os_queue_send_without_wait(msg_queue, msg))
	{
		os_mutex_give(main_mutex);
		return RC_DRIVER_BUSY | runtime_handle->state;
	}
	os_mutex_give(main_mutex);
	return RC_OK;
}


static uint8_t reinit_i2c_registers(struct NAU83GXX_config_t *config_handle,
						struct NAU83GXX_runtime_t *runtime_handle)
{
	struct task_message_t msg;

	msg.msg_type = MSG_TYPE_REINIT_I2C_REGS;
	return send_msg_and_wait(runtime_handle, &msg);
}


static uint8_t init_hw_with_full_kcs(struct NAU83GXX_config_t *config_handle,
						struct NAU83GXX_runtime_t *runtime_handle,
						struct hw_is_ready_ioctl_t *hw_is_ready_ioctl)
{
	struct task_message_t msg;
	struct init_hw_msg_t  *p_init_hw_msg;

	msg.msg_type = MSG_TYPE_INIT_HW;
	p_init_hw_msg = &msg.init_hw_msg;
	p_init_hw_msg->init_sel = INIT_WITH_REG_AND_KCS;
	p_init_hw_msg->biquad_init_arr = hw_is_ready_ioctl->biquad_init_arr;
	p_init_hw_msg->biquad_init_arr_size =
			hw_is_ready_ioctl->biquad_init_arr_size;
	p_init_hw_msg->kcs_spkr_param_data = hw_is_ready_ioctl->kcs_spkr_param_data;
	p_init_hw_msg->kcs_spkr_param_size = hw_is_ready_ioctl->kcs_spkr_param_size;
	p_init_hw_msg->end_of_ioctl_callback =
			hw_is_ready_ioctl->end_of_ioctl_callback;
	return send_msg_no_wait(runtime_handle, &msg);
}


static void init_driver(struct dev_desc_t *adev,
		struct NAU83GXX_config_t *config_handle,
		struct NAU83GXX_runtime_t *runtime_handle)
{
	if (STATE_DRIVER_INIT_DONE == runtime_handle->state) return;

	runtime_handle->main_mutex = os_create_mutex();
	if (NULL == runtime_handle->main_mutex)
	{
		CRITICAL_ERROR("cannot create mutex");
	}

	runtime_handle->task_handle = os_create_task("nau83gxx", nau83gxx_task,
					adev, NAU83GXX_TASK_STACK_SIZE , NAU83GXX_TASK_PRIORITY);

	runtime_handle->do_recovery = 0;
	runtime_handle->state = STATE_DRIVER_INIT_DONE;
}


static uint8_t init_hw_with_hw_reset_only(
		struct NAU83GXX_config_t *config_handle,
		struct NAU83GXX_runtime_t *runtime_handle)
{
	struct task_message_t msg;
	struct init_hw_msg_t  *p_init_hw_msg;

	msg.msg_type = MSG_TYPE_INIT_HW;
	p_init_hw_msg = &msg.init_hw_msg;
	p_init_hw_msg->init_sel = INIT_WITH_RESET_ONLY;
	return send_msg_no_wait(runtime_handle, &msg);
}


uint8_t check_correct_state(const uint8_t aIoctl_num, uint8_t state)
{
	switch(state)
	{
	case STATE_NOT_INITIALIZED:
		if (IOCTL_DEVICE_START != aIoctl_num) return RC_INTERNAL_ERROR;
		break;
	case STATE_DRIVER_INIT_DONE:
		switch(aIoctl_num)
		{
		case IOCTL_HW_IS_READY_TO_INIT_WITH_HW_RESET_ONLY:
		case IOCTL_HW_IS_READY_TO_INIT_WITH_FULL_KCS:
			break;
		default:
			return RC_INTERNAL_ERROR;
		}
		break;
	default:
		break;
	}
	return RC_OK;
}




static uint8_t send_kcs_simple_cmd(struct NAU83GXX_runtime_t *runtime_handle,
						struct kcs_simple_cmd_ioctl_t  *kcs_simple_cmd_ioctl)
{
	struct task_message_t msg;
	struct kcs_simple_cmd_msg_t  *p_kcs_simple_cmd_msg;

	msg.msg_type = MSG_KCS_I2C_SIMPLE_CMD;
	p_kcs_simple_cmd_msg = &msg.kcs_simple_cmd_msg;
	p_kcs_simple_cmd_msg->cmd = kcs_simple_cmd_ioctl->cmd;
	p_kcs_simple_cmd_msg->p_recieved_U32_reply =
			kcs_simple_cmd_ioctl->p_recieved_U32_reply;
	return send_msg_and_wait(runtime_handle, &msg);
}


static uint8_t send_kcs_get_cmd(struct NAU83GXX_runtime_t *runtime_handle,
						struct kcs_cmd_get_ioctl_t  *kcs_cmd_get_ioctl,
						uint8_t cmd)
{
	struct task_message_t msg;
	struct kcs_get_cmd_msg_t  *p_kcs_get_cmd_msg;

	msg.msg_type = MSG_KCS_I2C_GET_CMD;
	p_kcs_get_cmd_msg = &msg.kcs_get_cmd_msg;
	p_kcs_get_cmd_msg->cmd = cmd;
	p_kcs_get_cmd_msg->offset = kcs_cmd_get_ioctl->offset;
	p_kcs_get_cmd_msg->size_to_read = kcs_cmd_get_ioctl->size_to_read;
	p_kcs_get_cmd_msg->recieved_size = kcs_cmd_get_ioctl->recieved_size;
	p_kcs_get_cmd_msg->recieved_data = kcs_cmd_get_ioctl->recieved_data;
	return send_msg_and_wait(runtime_handle, &msg);
}


static uint8_t send_kcs_exit_get_state(
		struct NAU83GXX_runtime_t *runtime_handle)
{
	struct task_message_t msg;

	msg.msg_type = MSG_KCS_EXIT_GET_STATE;
	return send_msg_and_wait(runtime_handle, &msg);
}


static uint8_t send_start_collect_data(
	struct NAU83GXX_runtime_t *runtime_handle,
	struct kcs_start_collect_data_for_send_ioctl_t *p_start_collect_data_ioctl)
{
	struct start_collect_data_for_send_msg_t  *p_collect_data_msg;
	struct task_message_t msg;

	msg.msg_type = MSG_START_COLLECT_DATA_FOR_SEND;
	p_collect_data_msg = &msg.start_collect_data_for_send_msg;
	p_collect_data_msg->offset = p_start_collect_data_ioctl->offset;
	p_collect_data_msg->size_to_collect =
				p_start_collect_data_ioctl->size_to_collect;
	return send_msg_and_wait(runtime_handle, &msg);
}


static uint8_t send_add_data_msg(
	struct NAU83GXX_runtime_t *runtime_handle,
	struct kcs_add_data_for_send_ioctl_t *p_add_data_ioctl)
{
	struct add_data_for_send_msg_t  *p_add_data_msg;
	struct task_message_t msg;

	msg.msg_type = MSG_ADD_DATA_FOR_SEND;
	p_add_data_msg = &msg.add_data_for_send_msg;
	p_add_data_msg->data = p_add_data_ioctl->data;
	p_add_data_msg->size = p_add_data_ioctl->size;
	return send_msg_and_wait(runtime_handle, &msg);
}


static uint8_t send_kcs_send_collected_setup_data(
				struct NAU83GXX_runtime_t *runtime_handle)
{
	struct task_message_t msg;
	msg.msg_type = MSG_SEND_COLLECTED_SETUP;
	return send_msg_and_wait(runtime_handle, &msg);
}


static uint8_t send_power_down(struct NAU83GXX_runtime_t *runtime_handle)
{
	struct reply_message_t release_wait_msg;
	os_queue_t msg_queue;
	os_queue_t wait_for_finish_queue;
	os_mutex_t main_mutex;
	struct task_message_t msg;

	msg_queue = runtime_handle->msg_queue;
	wait_for_finish_queue = runtime_handle->wait_for_finish_queue;
	if ((NULL == msg_queue) || (NULL == wait_for_finish_queue))
	{
		return RC_DRIVER_NOT_READY | runtime_handle->state;
	}

	main_mutex = runtime_handle->main_mutex;
	os_mutex_take_infinite_wait(main_mutex);

	// clean wait queue
	os_queue_receive_with_timeout(wait_for_finish_queue, &release_wait_msg, 0);

	msg.msg_type = MSG_POWER_DOWN;
	os_queue_send_infinite_wait(msg_queue, &msg);
	if( OS_QUEUE_RECEIVE_SUCCESS != os_queue_receive_infinite_wait(
					wait_for_finish_queue, &release_wait_msg))
	{
		os_mutex_give(main_mutex);
		return RC_DRIVER_BUSY | runtime_handle->state;
	}

	os_delete_task(runtime_handle->task_handle);
	os_queue_delete(wait_for_finish_queue);
	runtime_handle->msg_queue = NULL;
	os_queue_delete(msg_queue);

	os_mutex_give(main_mutex);
	os_delay_ms(5); // wait for all tasks to stop waiting on mutex
	os_delete_mutex(main_mutex);

	return RC_OK;
}



static uint8_t send_kcs_send_setup_non_blocking(
	struct NAU83GXX_runtime_t *runtime_handle,
	struct kcs_send_setup_non_blocking_ioctl_t *p_setup_data_ioctl)
{
	struct send_setup_data_msg_t  *p_send_setup_data_msg;
	struct task_message_t msg;

	msg.msg_type = MSG_SEND_SETUP;
	p_send_setup_data_msg = &msg.send_setup_data_msg;
	p_send_setup_data_msg->data = p_setup_data_ioctl->data;
	p_send_setup_data_msg->size = p_setup_data_ioctl->size;
	p_send_setup_data_msg->offset = p_setup_data_ioctl->offset;
	return send_msg_no_wait(runtime_handle, &msg);
}


static uint8_t get_info(
	struct NAU83GXX_config_t *config_handle,
	struct NAU83GXX_runtime_t *runtime_handle,
	struct NAU83GXX_get_info_t *p_get_info)
{
	p_get_info->chip_type = config_handle->chip_type;

	if (STATE_IDLE > runtime_handle->state)
	{
		p_get_info->chip_type = NAU83GXX_CHIP_STATE_INITIALIZING;
	}
	else
	{
		p_get_info->chip_type = NAU83GXX_CHIP_STATE_RUNNING;
	}
	return RC_OK;
}


/**
 * kcs_i2c_ioctl()
 *
 * return:
 */
static uint8_t NAU83GXX_ioctl(struct dev_desc_t *adev,
		const uint8_t aIoctl_num, void *aIoctl_param1, void *aIoctl_param2)
{
	struct NAU83GXX_config_t *config_handle;
	struct NAU83GXX_runtime_t *runtime_handle;
	uint8_t ret_val;

	config_handle = DEV_GET_CONFIG_DATA_POINTER(NAU83GXX, adev);
	runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(NAU83GXX, adev);

	ret_val = check_correct_state(aIoctl_num, runtime_handle->state);
	if (ret_val) return ret_val;

	switch(aIoctl_num)
	{
	case IOCTL_DEVICE_START:
		init_driver(adev, config_handle, runtime_handle);
		break;
	case IOCTL_HW_IS_READY_TO_INIT_WITH_FULL_KCS:
		return init_hw_with_full_kcs(config_handle, runtime_handle,
				aIoctl_param1);
	case IOCTL_HW_IS_READY_TO_INIT_WITH_HW_RESET_ONLY:
		return init_hw_with_hw_reset_only(config_handle, runtime_handle);
	case IOCTL_NAU83GXX_REINIT_I2C_REGISTERS:
		return reinit_i2c_registers(config_handle, runtime_handle);
	case IOCTL_KCS_SIMPLE_CMD:
		return send_kcs_simple_cmd(runtime_handle, aIoctl_param1);
	case IOCTL_KCS_GET_SETUP_CMD:
		return send_kcs_get_cmd(
				runtime_handle, aIoctl_param1, DSP_CMD_GET_KCS_SETUP);
	case IOCTL_KCS_GET_RESULTS_CMD:
		return send_kcs_get_cmd(
				runtime_handle, aIoctl_param1, DSP_CMD_GET_KCS_RESULTS);
	case IOCTL_KCS_START_COLLECT_DATA_FOR_SEND:
		return send_start_collect_data(runtime_handle, aIoctl_param1);
	case IOCTL_KCS_ADD_DATA_FOR_SEND:
		return send_add_data_msg(runtime_handle, aIoctl_param1);
	case IOCTL_KCS_EXIT_GET_STATE:
		return send_kcs_exit_get_state(runtime_handle);
	case IOCTL_KCS_SEND_COLLECTED_SETUP_DATA:
		return send_kcs_send_collected_setup_data(runtime_handle);
	case IOCTL_KCS_SEND_SETUP_NON_BLOCKING:
		return send_kcs_send_setup_non_blocking(runtime_handle, aIoctl_param1);
	case IOCTL_NAU83GXX_GET_INFO:
		return get_info(config_handle, runtime_handle, aIoctl_param1);
	case IOCTL_DEVICE_STOP:
		return send_power_down(runtime_handle);
	default :
		return 1;
	}
	return 0;
}

#define	MODULE_NAME                      NAU83GXX
#define	MODULE_IOCTL_FUNCTION            NAU83GXX_ioctl
#define	MODULE_CALLBACK_FUNCTION         NAU83GXX_callback
#include "add_module.h"
