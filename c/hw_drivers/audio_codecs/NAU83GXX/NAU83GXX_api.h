
#ifndef _KCS_I2C_API_H_
#define _KCS_I2C_API_H_

#include "dev_management_api.h"
#include "os_wrapper.h"

#define NAU83GXX_CHIP_TYPE_G10        0xE0
#define NAU83GXX_CHIP_TYPE_G20        0x60

#define NAU83GXX_IN_CHANNEL_I2S_CHANNEL_0      0
#define NAU83GXX_IN_CHANNEL_TDM_CHANNEL_0      0
#define NAU83GXX_IN_CHANNEL_I2S_LEFT   NAU83GXX_IN_CHANNEL_I2S_CHANNEL_0
#define NAU83GXX_IN_CHANNEL_I2S_CHANNEL_1      1
#define NAU83GXX_IN_CHANNEL_TDM_CHANNEL_1      1
#define NAU83GXX_IN_CHANNEL_I2S_RIGHT   NAU83GXX_IN_CHANNEL_I2S_CHANNEL_1
#define NAU83GXX_IN_CHANNEL_TDM_CHANNEL_2      2
#define NAU83GXX_IN_CHANNEL_TDM_CHANNEL_3      3
#define NAU83GXX_IN_CHANNEL_TDM_CHANNEL_4      4
#define NAU83GXX_IN_CHANNEL_TDM_CHANNEL_5      5
#define NAU83GXX_IN_CHANNEL_TDM_CHANNEL_6      6
#define NAU83GXX_IN_CHANNEL_TDM_CHANNEL_7      7

#define NAU83GXX_OUT_CHANNEL_MUTE           1
#define NAU83GXX_OUT_CHANNEL_I_SENSE        2
#define NAU83GXX_OUT_CHANNEL_V_SENSE        3
#define NAU83GXX_OUT_CHANNEL_AEC            4


#define NAU83GXX_DSP_REG         0xF000
#define SIZE_OF_STD_DSP_WORD     4
#define DSP_COMM_IDLE_WORD       0xF4F3F2F1
#define DSP_COMM_PREAMBLE        0xA1B2
#define DSP_SPKR_PARAM_MAX_BYTES 972

#define KCS_I2C_MAX_BUFFER_IN_BYTES        ( 4095 )
#define FULL_KCS_DIAG_READ                 ( 3876 )
//#define KCS_RC_DEBUG_COUNTER

struct S_I2CCMD {
	size_t  u8Reg;
	uint8_t  u8Value[2];
};

typedef void (*end_of_ioctl_callback_f)(uint8_t);

#define L_CH_DEV_ADDR 0x10
#define R_CH_DEV_ADDR 0x11
#define I2C_DEV_DESC 0x46


#ifdef KCS_RC_DEBUG_COUNTER
	extern uint32_t kcs_rc_debug_counter;
#endif


/*
 * Task replies. This is what the task may say back to you as a sort of
 * error message. This is returned by use of the RTOS when the kcs_i2c API is
 * finished.
 */

enum kcs_i2c_return_code_t {
	RC_OK                         = 0x00,
	//Messages for KCS Device
	RC_COMMUNICATION_ERROR        = 0x01,
	RC_PREAMBLE_NOT_FOUND         = 0x02,
	RC_REPLY_LEN_TOO_LONG         = 0x03,
	RC_REPLY_DATA_INTEGRITY_ERROR = 0x04,
	RC_TIMEDOUT_FOR_IDLE_WORD     = 0x05,
	RC_REPLY_STATUS_ERR           = 0x10,
	//Messages for Data Handling
	RC_BUFFER_OVERFLOW_ERR        = 0x20,
	RC_INPUT_SIZE_TOO_LARGE_ERR   = 0x21,
	RC_BUFFER_NOT_EQUAL_TO_SIZE   = 0x22,
	//Messages for  driver
	RC_UNEXPECTED_REPLY           = 0x23,
	RC_INTERNAL_ERROR             = 0x24,
	RC_SYNTAX_ERROR               = 0x30,
	RC_DEVICE_DOES_NOT_EXIST      = 0x31,
	RC_PARAMETERS_OUT_OF_RANGE    = 0x32,
	//Messages for Filesystem Interface
	RC_FILESYSTEM_NOT_FOUND       = 0x33,
	RC_FILE_READ_FAILED           = 0x34,
	RC_FILE_WRITE_FAILED          = 0x35,

	RC_I2C_ERROR                  = 0x43,

	//Messages for Driver State (muxed with state)
	RC_DRIVER_NOT_READY           = 0xA0,
	RC_DRIVER_BUSY                = 0xB0,
	RC_OS_QUEUE_TIMEOUT           = 0xEF
};


enum kcs_i2c_cmd_e {
	DSP_CMD_ECHO_CNT 			= 0x01,
	DSP_CMD_GET_STATUS			= 0x09,
	DSP_CMD_GET_SW_REVISION		= 0x0A,
	DSP_CMD_GET_KCS_RESULTS   	= 0x04,
	DSP_CMD_GET_KCS_SETUP       = 0x06,
	DSP_CMD_SET_KCS_SETUP 		= 0x07,
	DSP_CMD_CLK_STOP            = 0x0B,
	DSP_CMD_CLK_RESTART         = 0x0C
};


enum kcs_i2c_ioctl_e {
	IOCTL_HW_IS_READY_TO_INIT_WITH_FULL_KCS = IOCTL_LAST_COMMON_IOCTL + 1,
	IOCTL_HW_IS_READY_TO_INIT_WITH_HW_RESET_ONLY,
	IOCTL_REINIT_I2C_REGISTERS,
	IOCTL_KCS_SIMPLE_CMD,
	IOCTL_KCS_GET_SETUP_CMD,
	IOCTL_KCS_GET_RESULTS_CMD,
	IOCTL_KCS_EXIT_GET_STATE,
	IOCTL_KCS_START_COLLECT_DATA_FOR_SEND,
	IOCTL_KCS_ADD_DATA_FOR_SEND,
	IOCTL_KCS_SEND_COLLECTED_SETUP_DATA,
	IOCTL_KCS_SEND_SETUP_NON_BLOCKING,
};

struct hw_is_ready_ioctl_t {
	const struct S_I2CCMD *biquad_init_arr;
	uint16_t biquad_init_arr_size;
	uint8_t *kcs_spkr_param_data;
	size_t kcs_spkr_param_size;
	end_of_ioctl_callback_f  end_of_ioctl_callback;
};


struct kcs_cmd_get_ioctl_t {
	uint16_t        offset;
	uint16_t        size_to_read;
	uint16_t        *recieved_size;
	uint8_t         **recieved_data;
};

struct kcs_simple_cmd_ioctl_t {
	uint8_t         cmd;
	uint32_t        *p_recieved_U32_reply;
};

struct kcs_start_collect_data_for_send_ioctl_t {
	uint16_t        offset;
	uint16_t        size_to_collect;
};

struct kcs_add_data_for_send_ioctl_t {
	uint8_t const   *data;
	uint16_t        size;
};


struct kcs_send_setup_non_blocking_ioctl_t {
	uint16_t        offset;
	uint8_t         *data;
	uint16_t        size;
};


struct NAU83GXX_config_t {
	struct dev_desc_t * i2c_dev;
	uint16_t boost_target_limit_mv;
	uint16_t boost_margin_mv;
	uint16_t current_limit_8ohm;
	uint8_t dev_addr;
	uint8_t chip_type;
	uint8_t I2S_in_channel;
	uint8_t I2S_out_left_ch_data_type;
	uint8_t I2S_out_right_ch_data_type;
	uint8_t enable_recovery;
	struct dev_desc_t *irq_pin;
};

SET_CONFIG_TYPE(NAU83GXX, struct NAU83GXX_config_t);

#endif
