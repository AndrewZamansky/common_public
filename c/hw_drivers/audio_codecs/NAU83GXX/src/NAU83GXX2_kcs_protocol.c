/*
 *
 * file :   NAU83GXX_kcs_protocol.c
 *
 *
 */

#ifndef FOR_EXPORT
	#include "_project.h"
#else
	#include "hal.h"
#endif
#include "os_wrapper.h"

//#define DEBUG
#include "PRINTF_api.h"

#include "NAU83GXX_api.h"
#include "NAU83GXX.h"



#define KCS_I2C_DSP_MAX_TX_SIZE            ( 96 )

#define SIZE_OF_STD_DSP_WORD     4
#define DSP_COMM_IDLE_WORD       0xF4F3F2F1
#define DSP_COMM_PREAMBLE        0xA1B2


static uint8_t send_simple_cmd(struct NAU83GXX_config_t *config_handle,
	struct NAU83GXX_runtime_t *runtime_handle, uint8_t cmd, uint32_t *response);
static uint8_t patch_for_correct_setup_write(
				struct NAU83GXX_config_t *config_handle,
				struct NAU83GXX_runtime_t *runtime_handle);


volatile uint8_t _dummy;
static void i2c_workaround_delay(struct NAU83GXX_runtime_t *runtime_handle)
{
	runtime_handle->_delay = 2000;
	while (runtime_handle->_delay--)
	{
		_dummy++;
	}
}


static uint8_t kcs_i2c_write(struct NAU83GXX_config_t *config_handle,
							struct NAU83GXX_runtime_t *runtime_handle,
					uint8_t const *write_data, size_t write_data_size)
{
	uint8_t rc;
	rc = nau83gxx_write(config_handle->i2c_dev, runtime_handle->dev_addr,
			runtime_handle->dsp_core_address, write_data, write_data_size);
	if (NAU83GXX_CHIP_TYPE_G60 == runtime_handle->chip_type)
	{// workaroud for corrupted (duplicated) data in I2C buffer when DSP is busy
//		os_delay_ms(1);
		i2c_workaround_delay(runtime_handle);
	}
	return rc;
}


static uint8_t kcs_i2c_read(struct NAU83GXX_config_t *config_handle,
		struct NAU83GXX_runtime_t *runtime_handle, uint8_t *read_data)
{
	uint8_t rc;

	rc = nau83gxx_read(config_handle->i2c_dev, runtime_handle->dev_addr,
			runtime_handle->dsp_core_address, read_data, SIZE_OF_STD_DSP_WORD);
	if (NAU83GXX_CHIP_TYPE_G60 == runtime_handle->chip_type)
	{// workaroud for corrupted (duplicated) data in I2C buffer when DSP is busy
//		os_delay_ms(1);
		i2c_workaround_delay(runtime_handle);
	}

	return rc;
}


static uint8_t check_if_idle(struct NAU83GXX_config_t *config_handle,
		struct NAU83GXX_runtime_t *runtime_handle)
{
	int retries;
	uint8_t rc;
	uint32_t u32Data;

	for (retries = 10; retries > 0; retries--)
	{
		rc = kcs_i2c_read(config_handle, runtime_handle, (uint8_t *)&u32Data);
		if(rc)
		{
			// communication error
			PRINTF_DBG("IDLE_WORD NAU83GXX_RC_COMMUNICATION_ERROR\r\n");
			continue;
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
		PRINTF_DBG("NAU83GXX_RC_TIMEDOUT_FOR_IDLE_WORD\r\n");
		return NAU83GXX_RC_TIMEDOUT_FOR_IDLE_WORD;
	}
	return NAU83GXX_RC_OK;
}


static uint8_t get_preamble(struct NAU83GXX_config_t *config_handle,
		struct NAU83GXX_runtime_t *runtime_handle, uint16_t *lenInU32)
{
	int retries;
	uint8_t replyStatus = 0;
	uint8_t rc = NAU83GXX_RC_OK;
	uint8_t data[SIZE_OF_STD_DSP_WORD] = {0};

	for (retries = 10; retries > 0; retries--)
	{
		rc = kcs_i2c_read(config_handle, runtime_handle, data);
		if (rc)
		{
			// communication error
			PRINTF_DBG("PREAMBLE NAU83GXX_RC_COMMUNICATION_ERROR\r\n");
			continue;
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
		PRINTF_DBG("NAU83GXX_RC_PREAMBLE_NOT_FOUND\r\n");
		return NAU83GXX_RC_PREAMBLE_NOT_FOUND;
	}

	// Check they reply from the DSP
	if(replyStatus != 0) return (NAU83GXX_RC_REPLY_STATUS_ERR | replyStatus);
	else return rc;
}


static uint8_t send_header_and_update_lenU32(
		struct NAU83GXX_config_t *config_handle,
		struct NAU83GXX_runtime_t *runtime_handle,
		uint8_t cmd, uint16_t sendLen, uint16_t *msg_len_u32, uint16_t *padding)
{
	uint8_t rc;
	uint16_t lenInU32;
	uint8_t data[4];

	//get idle phrase
	rc = check_if_idle(config_handle, runtime_handle);
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

	rc = kcs_i2c_write(
			config_handle, runtime_handle, data, SIZE_OF_STD_DSP_WORD);
	if(rc)
	{
		// communication error
		PRINTF_DBG("NAU83GXX_RC_COMMUNICATION_ERROR preamble\r\n");
		return NAU83GXX_RC_COMMUNICATION_ERROR;
	}
	*msg_len_u32 = lenInU32;
	return NAU83GXX_RC_OK;
}


static uint8_t send_offset_and_length(
		struct NAU83GXX_config_t *config_handle,
		struct NAU83GXX_runtime_t *runtime_handle,
		uint16_t data_len, uint16_t addr_offset)
{
	uint8_t rc;
	uint8_t data[SIZE_OF_STD_DSP_WORD] = {0};

	if(0 == data_len) return NAU83GXX_RC_OK; // nothing to read

	data[0] = addr_offset & 0xFF;
	data[1] = (addr_offset >> 8) & 0xFF;
	data[2] = data_len & 0xFF;
	data[3] = (data_len >> 8) & 0xFF;

	// send fragment
	rc = kcs_i2c_write(config_handle, runtime_handle, data, 4);
	if (rc)
	{
		// communication error
		return NAU83GXX_RC_COMMUNICATION_ERROR;
	}
	return rc;
}


static uint8_t send_payload(struct NAU83GXX_config_t *config_handle,
				struct NAU83GXX_runtime_t *runtime_handle,
				const uint8_t *dataSend, uint16_t sendLen)
{
	int i,j, data_size;
	uint8_t rc;
	uint16_t lenInU32;
	uint16_t left_size;
	uint8_t data[SIZE_OF_STD_DSP_WORD] = {0};

	if (0 == sendLen) return NAU83GXX_RC_OK;

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
		rc = kcs_i2c_write(config_handle, runtime_handle, data, data_size);
		if(rc)
		{
			// communication error
			PRINTF_DBG(
				"PAYLOAD NAU83GXX_RC_COMMUNICATION_ERROR fragment %d\r\n",i);
			return NAU83GXX_RC_COMMUNICATION_ERROR;
		}
		left_size -= 4;
	}
	return NAU83GXX_RC_OK;
}


static uint8_t get_reply(struct NAU83GXX_config_t *config_handle,
		struct NAU83GXX_runtime_t *runtime_handle,
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
		return NAU83GXX_RC_UNEXPECTED_REPLY;
	}

	*recvLen = 0;

	for (i = 0;  i < lenInU32 - 1; ++i)
	{
		// get fragment
		rc = kcs_i2c_read(config_handle, runtime_handle, data);
		if(rc)
		{
			// communication error
			PRINTF_DBG("REPLY NAU83GXX_RC_COMMUNICATION_ERROR\r\n");
			return NAU83GXX_RC_COMMUNICATION_ERROR;
		}
		for (j = 0; j < SIZE_OF_STD_DSP_WORD; ++j)
		{
			if( (i * 4 + j) < reqRecvLen)
				dataRecv[ (i * 4) + j ] =  data[j];
		}
	}

	//read last word (postamble)
	rc = kcs_i2c_read(config_handle, runtime_handle, data);
	if(rc)
	{
		// communication error
		PRINTF_DBG("TRAILING FRAGMENT NAU83GXX_RC_COMMUNICATION_ERROR\r\n");
		return NAU83GXX_RC_COMMUNICATION_ERROR;
	}

	verifyLen = data[0] + ((data[1] & 0xc0) << 2);
	if(verifyLen != lenInU32)
	{
		PRINTF_DBG("ERROR LEN_POST = %02X, expected %02X\r\n",
				verifyLen,lenInU32);
		return NAU83GXX_RC_REPLY_DATA_INTEGRITY_ERROR;
	}

	padding =  (data[1] & 0x30) >> 4;
	*recvLen = (lenInU32 - 1) * 4 - padding;

	return NAU83GXX_RC_OK;
}


static uint8_t send_postamble_and_get_reply(
			struct NAU83GXX_config_t *config_handle,
			struct NAU83GXX_runtime_t *runtime_handle,
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
		rc = kcs_i2c_write(
				config_handle, runtime_handle, data, SIZE_OF_STD_DSP_WORD);
		if(rc)
		{
			// communication error
			PRINTF_DBG("NAU83GXX_RC_COMMUNICATION_ERROR postamble\r\n");
			return NAU83GXX_RC_COMMUNICATION_ERROR;
		}
	}

	// at this stage, command was send, now we will get the reply

	//get preamble
	rc = get_preamble(config_handle, runtime_handle, &lenInU32);
	if(rc) return rc;

	// if reply has data - handle data
	if(lenInU32 > 0)
	{
		rc = get_reply(config_handle,
				runtime_handle, lenInU32, reqRecvLen, dataRecv, recvLen);
		if(rc) return rc;
		if( *recvLen > reqRecvLen) return NAU83GXX_RC_REPLY_LEN_TOO_LONG;
	}
	return NAU83GXX_RC_OK;
}


static uint8_t patch_for_correct_setup_write(
							struct NAU83GXX_config_t *config_handle,
							struct NAU83GXX_runtime_t *runtime_handle)
{
	uint16_t lenInU32, padding;
	uint8_t tmp_rx_buf[4];
	uint16_t tmp_rx_len;
	uint8_t rc;

	rc = send_header_and_update_lenU32(
			config_handle, runtime_handle, DSP_CMD_GET_KCS_RESULTS,
			4, &lenInU32, &padding);
	if(rc) return rc;

	rc = send_offset_and_length(config_handle, runtime_handle, 4, 0);
	if(rc) return rc;

	return send_postamble_and_get_reply(config_handle,
			runtime_handle, lenInU32, padding, 4, tmp_rx_buf, &tmp_rx_len);
}


static uint8_t send_kcs_setup(
		struct NAU83GXX_config_t *config_handle,
		struct NAU83GXX_runtime_t *runtime_handle, uint16_t  addr_offset,
		const uint8_t *p_send_buffer, uint16_t  size_to_send)
{
	uint16_t lenInU32, padding;
	uint16_t  retries;
	uint8_t rc = NAU83GXX_RC_OK;
	uint16_t  send_len;

	if (KCS_I2C_MAX_BUFFER_IN_BYTES <= size_to_send)
	{
		return NAU83GXX_RC_INPUT_SIZE_TOO_LARGE_ERR;
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
			rc = send_header_and_update_lenU32(config_handle, runtime_handle,
					DSP_CMD_SET_KCS_SETUP, send_len + 4, &lenInU32, &padding);
			if(rc) continue;

			rc = send_offset_and_length(config_handle,
					runtime_handle, send_len, addr_offset);
			if(rc) continue;

			rc = send_payload(config_handle,
					runtime_handle, p_send_buffer, send_len);
			if(rc) continue;

			rc = send_postamble_and_get_reply(config_handle,
					runtime_handle, lenInU32, padding, 4, NULL, NULL);
			if(rc) continue;

		}
		if (rc) return rc; //if after repeats still error - exit
		//maybe needed after each write
		//	patch_for_correct_setup_write(config_handle, runtime_handle);

		p_send_buffer += send_len;
		size_to_send -= send_len;
		addr_offset += send_len;
	}
	while (NAU83GXX_RC_OK !=
			patch_for_correct_setup_write(config_handle, runtime_handle));
	return NAU83GXX_RC_OK;
}


static uint8_t send_simple_cmd(
		struct NAU83GXX_config_t *config_handle,
		struct NAU83GXX_runtime_t *runtime_handle,
		uint8_t cmd, uint32_t *response)
{
	uint16_t lenInU32, padding;
	uint16_t recieved_len;
	uint8_t response_data[4];
	uint8_t rc;

	rc = send_header_and_update_lenU32(
			config_handle, runtime_handle, cmd, 0, &lenInU32, &padding);
	if(rc) return rc;
	rc = send_postamble_and_get_reply(config_handle, runtime_handle,
			lenInU32, padding, 4, response_data, &recieved_len);
	if(rc) return rc;

	*response = (response_data[0]) | (response_data[1] << 8) |
			(response_data[2] << 16) | (response_data[3] << 24);
	return NAU83GXX_RC_OK;
}
