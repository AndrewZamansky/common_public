/*
 * file :   downsampling_by_int_filter.c
 *
 */


#include "_project_typedefs.h"
#include "_project_defines.h"

#include "dsp_management_api.h"
#include "common_dsp_api.h"

#include "downsampling_by_int_api.h"
#include "downsampling_by_int.h"
#include "auto_init_api.h"
#include "fir_filter_api.h"


char downsampling_by_int_module_name[] = "downsampling_by_int";


/**
 * downsampling_by_int_dsp()
 *
 * return:
 */
static void downsampling_by_int_dsp(struct dsp_module_inst_t *adsp)
{
	float *apCh1In  ;
	float *apCh1Out ;
	float *apCh_tmp ;// for intermediate results
	struct downsampling_by_int_instance_t *handle;
	size_t in_data_len ;
	size_t out_data_len ;
	void *p_downsampling_by_int_filter ;
	size_t factor ;
	uint8_t buff_is_zero_buffer;

	handle = (struct downsampling_by_int_instance_t *)adsp->handle;
	p_downsampling_by_int_filter = handle->p_downsampling_by_int_filter;
	factor = handle->factor;
	if((0 == factor) || (NULL == p_downsampling_by_int_filter))
	{
		CRITICAL_ERROR("downsampling not initialized ");
		return;
	}

	buff_is_zero_buffer = 1;
	dsp_get_input_buffer_from_pad(
			adsp, 0, (uint8_t**)&apCh1In, &in_data_len, &buff_is_zero_buffer);
	out_data_len = in_data_len / factor;
	dsp_get_dummy_buffer((uint8_t**)&apCh_tmp, in_data_len);
	dsp_get_output_buffer_from_pad(adsp, 0, (uint8_t**)&apCh1Out, out_data_len);

	downsampling_by_int_function(p_downsampling_by_int_filter,
			apCh1In, apCh_tmp, in_data_len, apCh1Out);
}


static void set_downsampling_params(
		struct downsampling_by_int_instance_t *handle,
		struct downsampling_by_int_api_set_params_t  *params)
{
	struct fir_filter_api_set_params_t fir_set_params;
	uint32_t  input_sample_rate_Hz;
	uint32_t  output_sample_rate_Hz;

	input_sample_rate_Hz = params->input_sample_rate_Hz;
	output_sample_rate_Hz = params->output_sample_rate_Hz;

	if (0 != (input_sample_rate_Hz % output_sample_rate_Hz))
	{
		CRITICAL_ERROR("downsampling factor should be integer");
	}
	handle->factor = input_sample_rate_Hz / output_sample_rate_Hz;

	fir_set_params.set_coefficients_type = FIR_CALCULATE_COEFFICIENTS_BY_PARAMS;
	fir_set_params.coeff_by_params.filter_mode = FIR_LOWPASS_MODE;
	fir_set_params.coeff_by_params.fc = output_sample_rate_Hz / 2;
	//fir_set_params.coeff_by_params.dfc = 1000;// not relevant for lowpass
	fir_set_params.coeff_by_params.A_stop = 90;
	fir_set_params.coeff_by_params.sample_rate_Hz = input_sample_rate_Hz;
	fir_set_params.number_of_filter_coefficients =
					params->number_of_coefficients_in_lowpass_filter;
	fir_set_params.expected_number_of_input_samples =
						params->expected_number_of_input_samples;
	downsampling_by_int_create(handle, &fir_set_params);
}


/**
 * downsampling_by_int_ioctl()
 *
 * return:
 */
static uint8_t downsampling_by_int_ioctl(struct dsp_module_inst_t *adsp,
		const uint8_t aIoctl_num, void * aIoctl_param1, void * aIoctl_param2)
{
	struct downsampling_by_int_instance_t *handle;

	handle = (struct downsampling_by_int_instance_t *)adsp->handle;
	switch(aIoctl_num)
	{
	case IOCTL_DSP_INIT :
		handle->factor =0;
		handle->p_downsampling_by_int_filter = NULL;
		break;

	case IOCTL_DOWNSAMPLING_BY_INT_SET_PARAMS :
		set_downsampling_params(handle,
				(struct downsampling_by_int_api_set_params_t *)aIoctl_param1);
		break;

	default :
		return 1;
	}
	return 0;
}


/**
 * downsampling_by_int_init()
 *
 * return:
 */
extern "C" void  downsampling_by_int_init(void)
{
	DSP_REGISTER_NEW_MODULE("downsampling_by_int",
			downsampling_by_int_ioctl,
			downsampling_by_int_dsp,
			NULL,
			dsp_management_default_mute,
			DSP_MANAGEMENT_FLAG_BYPASS_NOT_AVAILABLE,
			struct downsampling_by_int_instance_t);
}

AUTO_INIT_FUNCTION(downsampling_by_int_init);
