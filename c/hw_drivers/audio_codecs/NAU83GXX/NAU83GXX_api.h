
#ifndef _NAU83GXX_API_H_
#define _NAU83GXX_API_H_

#include "dev_management_api.h"
#include "os_wrapper.h"

#define NAU83GXX_CHIP_TYPE_AUTO_DETECT  0x00
#define NAU83GXX_CHIP_TYPE_G10          0xE0
#define NAU83GXX_CHIP_TYPE_G20          0x60
#define NAU83GXX_CHIP_TYPE_G60          0xF0

#define NAU83GXX_BUS_TYPE_I2S       0
#define NAU83GXX_BUS_TYPE_PCMA      1
#define NAU83GXX_BUS_TYPE_PCMB      2

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


#define NAU83GXX_DSP_CORE_0_REG             0xF000
#define NAU83GXX_DSP_CORE_1_REG             0xF002
#define KCS_I2C_MAX_BUFFER_IN_BYTES        ( 4095 )

struct NAU83GXX_reg_s {
	size_t  u8Reg;
	uint8_t  u8Value[2];
};

typedef void (*end_of_ioctl_callback_f)(uint8_t ret_code);


/*
 * Task replies. This is what the task may say back to you as a sort of
 * error message. This is returned by use of the RTOS when the kcs_i2c API is
 * finished.
 */

enum NAU83GXX_NAU83GXX_RC_e {
	NAU83GXX_RC_OK                         = 0x00,
	//Messages for KCS Device
	NAU83GXX_RC_COMMUNICATION_ERROR        = 0x01,
	NAU83GXX_RC_PREAMBLE_NOT_FOUND         = 0x02,
	NAU83GXX_RC_REPLY_LEN_TOO_LONG         = 0x03,
	NAU83GXX_RC_REPLY_DATA_INTEGRITY_ERROR = 0x04,
	NAU83GXX_RC_TIMEDOUT_FOR_IDLE_WORD     = 0x05,
	NAU83GXX_RC_ALC_SEGMENT_TOO_BIG        = 0x06,
	NAU83GXX_RC_ALC_WRONG_VBAT_MIN_MAX_LEVELS = 0x07,
	NAU83GXX_RC_REPLY_STATUS_ERR           = 0x10,

	NAU83GXX_RC_BUFFER_OVERFLOW_ERR        = 0x20,
	NAU83GXX_RC_INPUT_SIZE_TOO_LARGE_ERR   = 0x21,
	NAU83GXX_RC_BUFFER_NOT_EQUAL_TO_SIZE   = 0x22,
	NAU83GXX_RC_UNEXPECTED_REPLY           = 0x23,
	NAU83GXX_RC_DRIVER_IN_WRONG_STATE      = 0x24,
	NAU83GXX_RC_I2C_DATA_TOO_BIG           = 0x25,

	NAU83GXX_RC_SYNTAX_ERROR               = 0x30,
	NAU83GXX_RC_DEVICE_DOES_NOT_EXIST      = 0x31,
	NAU83GXX_RC_PARAMETERS_OUT_OF_RANGE    = 0x32,
	NAU83GXX_RC_NOT_SUPPORTED_DEVICE_ID    = 0x33,
	NAU83GXX_RC_NOT_SUPPORTED_IOCTL        = 0x34,
	NAU83GXX_RC_NOT_INTERNAL_DEVICE        = 0x35,

	NAU83GXX_RC_I2C_ERROR                  = 0x43,

	//Messages for Driver State (muxed with state)
	NAU83GXX_RC_DRIVER_NOT_READY           = 0xA0,
	NAU83GXX_RC_DRIVER_BUSY                = 0xB0,
	NAU83GXX_RC_OS_QUEUE_TIMEOUT           = 0xEF
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


enum NAU83GXX_IOCTL_e {
	IOCTL_HW_IS_READY_TO_INIT = IOCTL_LAST_COMMON_IOCTL + 1,
	IOCTL_NAU83GXX_REINIT_I2C_REGISTERS,
	IOCTL_KCS_REINIT_I2C_REGISTERS,
	IOCTL_KCS_SIMPLE_CMD,
	IOCTL_KCS_GET_SETUP_CMD,
	IOCTL_KCS_GET_RESULTS_CMD,
	IOCTL_KCS_EXIT_GET_STATE,
	IOCTL_KCS_START_COLLECT_DATA_FOR_SEND,
	IOCTL_KCS_ADD_DATA_FOR_SEND,
	IOCTL_KCS_SEND_COLLECTED_SETUP_DATA,
	IOCTL_KCS_SEND_SETUP_NON_BLOCKING,
	IOCTL_NAU83GXX_GET_INFO,
	IOCTL_NAU83GXX_BYPASS_KCS,
	IOCTL_NAU83GXX_BYPASS_BIQUADS,
	IOCTL_KCS_SET_ALC_DATA,
	IOCTL_KCS_REGISTER_READ,
	IOCTL_KCS_REGISTER_WRITE,
	IOCTL_NAU83GXX_GET_CURRENT_VOLTAGE_mV
};

struct hw_is_ready_ioctl_t {
	const struct NAU83GXX_reg_s *registers_init_arr;
	uint16_t registers_init_arr_num_of_items;
	const struct NAU83GXX_reg_s *biquad_init_arr;
	uint16_t biquad_init_arr_num_of_items;
	uint8_t *kcs_spkr_param_data;
	size_t kcs_spkr_param_size;
	end_of_ioctl_callback_f  end_of_ioctl_callback;
	uint8_t perform_only_hw_reset;
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


struct nau83gxx_bypass_kcs_ioctl_t {
	uint8_t   bypass; // 0 - don't bypass, 1-255 - bypass
};

struct nau83gxx_bypass_biquads_ioctl_t {
	uint8_t   bypass; // 0 - don't bypass, 1-255 - bypass
};

struct nau83gxx_get_current_voltage_t {
	uint32_t   cur_voltage_mV;
};

struct kcs_cmd_set_alc_data_ioctl_t {
	float max_battery_level;
	float min_battery_level_V;
	float battery_change_step_V;
	float max_kcs_gain;
	float min_kcs_gain;
	float max_limiter_voltage;
	float min_limiter_voltage;
	uint8_t alc_segment_num; // set 0xff to skip update of any segment
	uint8_t alc_enable;
};


struct kcs_cmd_register_read_ioctl_t {
	uint16_t reg_addr;
	uint32_t data;
};


struct kcs_cmd_register_write_ioctl_t {
	uint16_t reg_addr;
	uint32_t data;
};


enum NAU83GXX_chip_state_e {
	 NAU83GXX_CHIP_STATE_INITIALIZING = 0,
	 NAU83GXX_CHIP_STATE_RUNNING
};


struct NAU83GXX_get_info_t {
	uint8_t chip_type;
	uint8_t chip_state;
	uint8_t is_kcs_loaded;
};


struct NAU83GXX_config_t {
	struct dev_desc_t * i2c_dev;
	uint16_t boost_target_limit_mv;
	uint16_t boost_margin_mv;
	uint16_t current_limit_8ohm;
	uint8_t i2c_addr_arr_size;
	uint8_t *i2c_addr_arr;
	uint8_t chip_type;
	uint8_t I2S_bus_type;
	uint8_t I2S_in_channel;
	uint8_t I2S_word_length;
	uint8_t I2S_out_left_ch_data_type;
	uint8_t I2S_out_right_ch_data_type;
	uint8_t enable_recovery;
	struct dev_desc_t *irq_pin;
};
SET_CONFIG_TYPE(NAU83GXX, struct NAU83GXX_config_t);


struct KCS_config_t {
	struct dev_desc_t * nau83gxx;
	uint16_t dsp_core;
};
SET_CONFIG_TYPE(KCS, struct KCS_config_t);


#ifdef FOR_EXPORT
uint8_t dev_NAU83GXX_ioctl(struct dev_desc_t *adev,
		const uint8_t aIoctl_num, void *aIoctl_param);
uint8_t dev_NAU83GXX_ioctl(struct dev_desc_t *adev,
		const uint8_t aIoctl_num, void *aIoctl_param);
uint8_t dev_KCS_ioctl(struct dev_desc_t *adev,
		const uint8_t aIoctl_num, void *aIoctl_param);
#endif
			
#endif
