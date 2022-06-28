/*
 *
 * file :   NAU83GXX.h
 *
 *
 */

#ifndef _NAU83GXX_H
#define _NAU83GXX_H

#ifndef FOR_EXPORT
	#include "_project.h"
#else
	#include "hal.h"
#endif
#include "dev_management_api.h"
#include "os_wrapper.h"

#define NAU83GXX_REG_DEVICE_ID  0x46

#define  MAX_ALC_SEGMENTS  5

#define INTERNAL_DSP_ACCESS_MAGIC_NUMBER  0xE2D3C4B5

struct NAU83GXX_internal_dsp_access_t {
	uint8_t *null_pointer1; // to make sure that module data is NULL
	uint8_t *null_pointer2; // to make sure that config data is NULL
	uint32_t magic_number;
	struct NAU83GXX_config_t *config_handle;
	struct NAU83GXX_runtime_t *runtime_handle;
	uint16_t dsp_core_address;
};

uint8_t NAU83GXX_send_setup_data_from_kcs_remote_interface(
		struct NAU83GXX_internal_dsp_access_t *internal_dev,
		uint16_t offset, uint8_t const *data, uint16_t size);

struct NAU83GXX_alc_segment_t {
	float min_battery_level_V;
	float battery_change_step_V;
	float kcs_gain_a; //kcs_gain = bat_level * kcs_gain_a + kcs_gain_b
	float kcs_gain_b;
	float u_limit_a; //u_limit = bat_level * u_limit_a + u_limit_b
	float u_limit_b;
};

struct NAU83GXX_runtime_t {
	os_queue_t msg_queue;
	os_queue_t wait_for_finish_queue;
	os_mutex_t main_mutex;
	os_task_t  task_handle;
	const struct NAU83GXX_reg_s *registers_init_arr;

	//alc segments should be organized in decending order (by vbat voltage)
	struct NAU83GXX_alc_segment_t  alc_segment[MAX_ALC_SEGMENTS];
	float    vbat_V;
	uint16_t dsp_core_address;
	uint16_t registers_init_arr_num_of_items;
	uint16_t data_set_buff_pos;
	uint16_t addr_offset;
	uint16_t sendLen;
	uint8_t *dataBuf;
	uint8_t  state;
	uint8_t  do_recovery;
	uint8_t chip_type;
	uint8_t dev_addr;
	uint8_t is_kcs_loaded;
	uint8_t sw_alc_enabled;
	uint32_t _delay;
};
SET_RUNTIME_DATA_TYPE(NAU83GXX, struct NAU83GXX_runtime_t);

#endif
