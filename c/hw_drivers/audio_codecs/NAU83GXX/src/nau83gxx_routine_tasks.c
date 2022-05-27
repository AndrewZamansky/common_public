/*
 *
 * file :   NAU83GXX.c
 *
 *
 */

#ifndef FOR_EXPORT
	#include "_project.h"
#else
	#include "hal.h"
#endif
#include "dev_management_api.h"
#include "os_wrapper.h"

//#define DEBUG
#include "PRINTF_api.h"

#include "os_wrapper.h"
#include "NAU83GXX_api.h"
#include "NAU83GXX.h"
#include "math.h"

#ifdef CONFIG_NAU83GXX_USE_KCS_REMOTE_INTERFACE
	#include "CoreModules/KCS_RemoteInterface.h"
#endif

extern uint8_t nau83gxx_read_wordU16(struct dev_desc_t *i2c_dev,
		uint8_t device_addr, uint16_t reg_addr, uint16_t *read_dat);
extern uint8_t nau83gxx_write_wordU16(struct dev_desc_t *i2c_dev,
		uint8_t device_addr, uint16_t reg_addr, uint16_t write_dat);


static void perform_alc_adjustment(
		struct NAU83GXX_config_t *config_handle,
		struct NAU83GXX_runtime_t *runtime_handle,
		struct NAU83GXX_alc_segment_t *p_alc_segment,
		float curr_vbat_V, float prev_vbat_V)
{
#ifdef CONFIG_NAU83GXX_USE_KCS_REMOTE_INTERFACE
	float curr_vbat_change_v;
	struct NAU83GXX_internal_dsp_access_t  internal_dsp_access;
	float new_kcs_gain;
	float new_u_limit;
//	float kcs_gain_step;
//	float u_limit_step;
	Frac32 gain_frac32;
	PseudoFloatSI u_out_limit_PseudoFloatSI;
	uint16_t i;

	if (0 == p_alc_segment->min_battery_level_V) return;

	if (curr_vbat_V > prev_vbat_V)
	{
		curr_vbat_change_v = curr_vbat_V - prev_vbat_V;
	}
	else
	{
		curr_vbat_change_v = prev_vbat_V - curr_vbat_V;
	}

	if (curr_vbat_change_v <= p_alc_segment->battery_change_step_V) return;

	internal_dsp_access.null_pointer1 = NULL;
	internal_dsp_access.null_pointer2 = NULL;
	internal_dsp_access.magic_number = INTERNAL_DSP_ACCESS_MAGIC_NUMBER;
	internal_dsp_access.config_handle = config_handle;
	internal_dsp_access.runtime_handle = runtime_handle;
	internal_dsp_access.dsp_core_address = NAU83GXX_DSP_CORE_0_REG;

	new_kcs_gain =
		(curr_vbat_V * p_alc_segment->kcs_gain_a) + p_alc_segment->kcs_gain_b;
	new_kcs_gain = powf(10, (new_kcs_gain - 30.0f) / 20.0f);
	if (1 < new_kcs_gain) new_kcs_gain = 1;
	gain_frac32 = new_kcs_gain * 0x80000000;
	kcs_remote_set_audio_gain(&internal_dsp_access, gain_frac32);

	new_u_limit =
		(curr_vbat_V * p_alc_segment->u_limit_a) + p_alc_segment->u_limit_b;
	for (i = 0; i < 0xffff; i++)
	{
		if (1 > new_u_limit) break;
		new_u_limit = new_u_limit / 2;
	}
	u_out_limit_PseudoFloatSI.value = new_u_limit * 0x8000; // convert to Q0.15
	u_out_limit_PseudoFloatSI.shift = i;
	kcs_remote_set_output_limiter(
				&internal_dsp_access, u_out_limit_PseudoFloatSI);
#endif
}


static void nau83gxx_sw_alc(
		struct NAU83GXX_config_t *config_handle,
		struct NAU83GXX_runtime_t *runtime_handle)
{
	uint8_t ret_val;
	uint16_t val;
	float vbat_V;
	float prev_vbat_V;
	struct NAU83GXX_alc_segment_t *p_alc_segment;
	uint8_t i;

	// G60 ALC not implemented yet
	if (NAU83GXX_CHIP_TYPE_G60 == runtime_handle->chip_type) return;

	if (0 == runtime_handle->sw_alc_enabled) return;

	ret_val = nau83gxx_read_wordU16(config_handle->i2c_dev,
				runtime_handle->dev_addr, 0x20, &val);
	if (0 != ret_val) return;

	vbat_V = val & 0xff;
	vbat_V = (vbat_V * 0.0622) + 0.5869;

	prev_vbat_V = runtime_handle->vbat_V;
	runtime_handle->vbat_V = vbat_V;

	for (i = 0; i < MAX_ALC_SEGMENTS; i++)
	{
		p_alc_segment = &runtime_handle->alc_segment[i];
		if (vbat_V < p_alc_segment->min_battery_level_V)
		{
			continue;
		}
		perform_alc_adjustment(
			config_handle, runtime_handle, p_alc_segment, vbat_V, prev_vbat_V);
		break;
	}
}


void nau83gxx_sw_perform_routine_tasks(
		struct NAU83GXX_config_t *config_handle,
		struct NAU83GXX_runtime_t *runtime_handle)
{
	nau83gxx_sw_alc(config_handle, runtime_handle);
}

