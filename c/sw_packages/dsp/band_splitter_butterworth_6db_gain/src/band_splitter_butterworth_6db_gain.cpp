/*
 *
 * file :   band_splitter_butterworth_6db_gain.c
 *
 *
 */

#include "_project_typedefs.h"
#include "_project_defines.h"

#include "dsp_management_api.h"
#include "dsp_management_internal_api.h"


#include "band_splitter_butterworth_6db_gain_api.h"
#include "band_splitter_butterworth_6db_gain.h"

#include "auto_init_api.h"

#include "string.h"

#define DEBUG
#include "biquad_filter_api.h"

extern "C" {
	extern void band_splitter_butterworth_6db_gain_init_subchains(
			struct band_splitter_butterworth_6db_gain_instance_t *handle);
	extern char band_splitter_butterworth_6db_filter_all_pass_1_pole[];
	extern char band_splitter_butterworth_6db_filter_all_pass_2_poles[];
}

char band_splitter_butterworth_6db_gain_module_name[] =
										"band_splitter_butterworth_6db_gain";


static void band_splitter_butterworth_6db_gain_bypass(
									struct dsp_module_inst_t * adsp)
{
	size_t buff_size;
	uint8_t *in_buff;
	uint8_t *out_buff;
	uint8_t buff_is_zero_buffer;

	buff_is_zero_buffer = 1;
	dsp_get_input_buffer_from_pad(
			adsp, 0, &in_buff, &buff_size, &buff_is_zero_buffer);
	dsp_get_output_buffer_from_pad(adsp, 0, &out_buff, buff_size);
	memcpy(out_buff, in_buff, buff_size);
	dsp_get_output_buffer_from_pad(adsp, 1, &out_buff, buff_size);
	memcpy(out_buff, in_buff, buff_size);
}


/**
 * band_splitter_butterworth_6db_gain_dsp()
 *
 * return:
 */
static void band_splitter_butterworth_6db_gain_dsp(
						struct dsp_module_inst_t * adsp)
{
	struct band_splitter_butterworth_6db_gain_instance_t *handle;
	chain_handle_t filter_1_pole_dsp_chain;
	chain_handle_t filter_2_poles_dsp_chain;
	real_t *apCh1In ;
	real_t *apCh1Out  ;
	real_t *apCh2Out  ;
	uint16_t i;
	size_t data_len ;
	uint8_t buff_is_zero_buffer;

	buff_is_zero_buffer = 1;
	dsp_get_input_buffer_from_pad(
			adsp, 0, (uint8_t**)&apCh1In, &data_len, &buff_is_zero_buffer);
	dsp_get_output_buffer_from_pad(adsp, 0, (uint8_t**)&apCh1Out, data_len);
	dsp_get_output_buffer_from_pad(adsp, 1, (uint8_t**)&apCh2Out, data_len);

	handle =
		(struct band_splitter_butterworth_6db_gain_instance_t *)adsp->handle;

	filter_1_pole_dsp_chain = handle->filter_1_pole_dsp_chain;
	filter_2_poles_dsp_chain = handle->filter_2_poles_dsp_chain;
	dsp_management_api_set_chain_input_buffer(filter_1_pole_dsp_chain,
			IN_PAD(0), (uint8_t *)apCh1In, data_len);
	dsp_management_api_set_chain_output_buffer(filter_1_pole_dsp_chain,
			OUT_PAD(0), (uint8_t *)apCh1Out, data_len);
	dsp_management_api_process_chain(filter_1_pole_dsp_chain );

	dsp_management_api_set_chain_input_buffer(filter_2_poles_dsp_chain,
			IN_PAD(0), (uint8_t *)apCh1In, data_len);
	dsp_management_api_set_chain_output_buffer(filter_2_poles_dsp_chain,
			OUT_PAD(0), (uint8_t *)apCh2Out, data_len);
	dsp_management_api_process_chain(filter_2_poles_dsp_chain );

	data_len /= sizeof(real_t);
	for ( i = 0; i < data_len; i++)
	{
		real_t tmp;

		tmp = *apCh1Out + *apCh2Out;
		*apCh1Out = *apCh1Out - *apCh2Out;
		apCh1Out++;
		*apCh2Out++ = tmp;
	}
}


static 	void set_freq(
		struct band_splitter_butterworth_6db_gain_instance_t *handle,
		real_t freq)
{
	struct biquad_filter_api_band_set_t band_set;
	struct biquad_filter_api_band_set_params_t  *p_band_set_params;
	chain_handle_t filter_1_pole_dsp_chain;
	chain_handle_t filter_2_poles_dsp_chain;

	filter_1_pole_dsp_chain = handle->filter_1_pole_dsp_chain;
	filter_2_poles_dsp_chain = handle->filter_2_poles_dsp_chain;


	p_band_set_params = &band_set.band_set_params;
	p_band_set_params->bypass = 0;
	p_band_set_params->Fc = freq;
	p_band_set_params->filter_mode = BIQUADS_ALL_PASS_BUTTERWORTH_1_POLE;
	band_set.band_num = 0;
	DSP_IOCTL(filter_1_pole_dsp_chain,
			band_splitter_butterworth_6db_filter_all_pass_1_pole,
			IOCTL_BIQUAD_FILTER_SET_BAND, &band_set );
	p_band_set_params->filter_mode = BIQUADS_ALL_PASS_BUTTERWORTH_2_POLES;
	band_set.band_num = 0;
	DSP_IOCTL(filter_2_poles_dsp_chain,
			band_splitter_butterworth_6db_filter_all_pass_2_poles,
			IOCTL_BIQUAD_FILTER_SET_BAND, &band_set );
}



/**
 * band_splitter_butterworth_6db_gain_ioctl()
 *
 * return:
 */
static uint8_t band_splitter_butterworth_6db_gain_ioctl(
		struct dsp_module_inst_t * adsp,
		const uint8_t aIoctl_num, void * aIoctl_param1, void * aIoctl_param2)
{
	struct band_splitter_butterworth_6db_gain_instance_t *handle;
	struct band_splitter_butterworth_6db_gain_api_set_params_t *set_params;
	real_t freq;

	handle =
		(struct band_splitter_butterworth_6db_gain_instance_t *)adsp->handle;
	set_params = &handle->set_params;

	switch(aIoctl_num)
	{

	case IOCTL_DSP_INIT :
		set_params->fc = 20000.0f;
		band_splitter_butterworth_6db_gain_init_subchains(handle);
		break;

	case IOCTL_BAND_SPLITTER_BUTTERWORTH_6DB_GAIN_SET_FC :
		freq = *(float*)aIoctl_param1;
		set_params->fc = freq;
		set_freq(handle, freq);
		break;

	case IOCTL_BAND_SPLITTER_BUTTERWORTH_6DB_GAIN_GET_PARAMS :
		memcpy(aIoctl_param1, set_params,
			sizeof(struct band_splitter_butterworth_6db_gain_api_set_params_t));
		break;
	default :
		return 1;
	}
	return 0;
}


/**
 * band_splitter_butterworth_6db_gain_init()
 *
 * return:
 */
extern "C" void  band_splitter_butterworth_6db_gain_init(void)
{
	DSP_REGISTER_NEW_MODULE("band_splitter_butterworth_6db_gain",
			band_splitter_butterworth_6db_gain_ioctl,
			band_splitter_butterworth_6db_gain_dsp,
			band_splitter_butterworth_6db_gain_bypass,
			dsp_management_default_mute,
			0,	struct band_splitter_butterworth_6db_gain_instance_t);
}

AUTO_INIT_FUNCTION(band_splitter_butterworth_6db_gain_init);
