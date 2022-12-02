/*
 *
 * file :   biquad_filter.c
 *
 *
 */
#include "_project_typedefs.h"
#include "_project_defines.h"
extern "C" {
#include "errors_api.h"
#include "os_wrapper.h"
}

#include <string.h>
#include "dsp_management_api.h"
#include "dsp_management_internal_api.h"

#include "biquad_filter_api.h"
#include "biquad_filter.h"
#include "os_wrapper.h"

#include "auto_init_api.h"

char biquad_filter_module_name[] = "biquad_filter";


/**
 * biquad_filter_dsp()
 *
 * return:
 */
static void biquad_filter_dsp(struct dsp_module_inst_t *adsp)
{
	real_t *apCh1In  ;
	real_t *apCh1Out ;
	struct biquads_filter_t *handle;
	size_t data_len ;
	uint8_t buff_is_zero_buffer;

	buff_is_zero_buffer = 1;
	dsp_get_input_buffer_from_pad(
			adsp, 0, (uint8_t**)&apCh1In, &data_len, &buff_is_zero_buffer);
	dsp_get_output_buffer_from_pad(adsp, 0, (uint8_t**)&apCh1Out, data_len);

	handle = (struct biquads_filter_t *)adsp->handle;
	if(0 == handle->num_of_bands)
	{
		CRITICAL_ERROR("at least 1 band should be set");
	}

	biquads_cascading_filter(handle->pBiquadFilter,
			apCh1In, apCh1Out, data_len / sizeof(real_t));
}


static void set_number_of_bands(
		struct biquads_filter_t *handle, size_t num_of_bands)
{
	struct biquad_filter_api_band_set_params_t *p_band_set_params;
	uint16_t i;
	real_t *biquad_bands_coeffs;


	handle->num_of_bands = num_of_bands;
	os_safe_free(handle->biquad_bands_coeffs);
	os_safe_free(handle->band_set_params);

	biquad_bands_coeffs =
			(real_t *)os_safe_malloc(	5 * sizeof(real_t) * num_of_bands);
	errors_api_check_if_malloc_succeed(biquad_bands_coeffs);
	handle->biquad_bands_coeffs = biquad_bands_coeffs;

	p_band_set_params =
		(struct  biquad_filter_api_band_set_params_t *)os_safe_malloc(
			num_of_bands * sizeof(struct  biquad_filter_api_band_set_params_t));
	errors_api_check_if_malloc_succeed(p_band_set_params);
	handle->band_set_params = p_band_set_params;

	for (i = 0; i < num_of_bands; i++)
	{
		uint16_t curr_coeff_start_pos;

		curr_coeff_start_pos = i * 5;
		biquad_bands_coeffs[curr_coeff_start_pos + 0] = (float)1;
		biquad_bands_coeffs[curr_coeff_start_pos + 1] = (float)0;
		biquad_bands_coeffs[curr_coeff_start_pos + 2] = (float)0;
		biquad_bands_coeffs[curr_coeff_start_pos + 3] = (float)0;
		biquad_bands_coeffs[curr_coeff_start_pos + 4] = (float)0;
		p_band_set_params[i].filter_mode = BIQUADS_PEAK_MODE;
		p_band_set_params[i].Fc = 1000.0f;
		p_band_set_params[i].QValue = 0.707f;
		p_band_set_params[i].Gain_db = 0.0f;
		p_band_set_params[i].bypass = 1;
	}

	biquads_free(handle->pBiquadFilter);
	handle->pBiquadFilter =
			biquads_alloc(handle->num_of_bands, biquad_bands_coeffs );

}


/**
 * biquad_filter_ioctl()
 *
 * return:
 */
static uint8_t biquad_filter_ioctl(struct dsp_module_inst_t *adsp,
		const uint8_t aIoctl_num, void * aIoctl_param1 , void * aIoctl_param2)
{
	size_t num_of_bands;
	uint8_t band_num;
	struct biquad_filter_api_band_set_params_t *p_band_set_params;
	struct biquad_filter_api_band_set_raw_coefficients_t
													*p_band_set_coefficients;
	struct biquads_filter_t *handle;
	struct biquad_filter_api_band_set_t *band_params;
	enum biquads_filter_mode_e filter_mode;
	struct biquad_filter_api_band_set_raw_t *band_raw_params;

	real_t *curr_coeffs;

	handle = (struct biquads_filter_t *)adsp->handle;

	switch(aIoctl_num)
	{
	case IOCTL_DSP_INIT :
		handle->num_of_bands = 0;
		handle->biquad_bands_coeffs = NULL ;
		handle->band_set_params = NULL ;
		handle->pBiquadFilter = NULL ;
		break;

	case IOCTL_BIQUAD_FILTER_SET_NUM_OF_BANDS :
		num_of_bands = ((size_t)aIoctl_param1);
		set_number_of_bands(handle, num_of_bands);
		break;

	case IOCTL_BIQUAD_FILTER_SET_BAND :
		band_params = ((struct  biquad_filter_api_band_set_t*)aIoctl_param1);
		num_of_bands = handle->num_of_bands;
		band_num = band_params->band_num ;
		p_band_set_params = &(band_params->band_set_params);

		if ((num_of_bands <= band_num ) || (p_band_set_params->Fc < 0.01f))
		{
			CRITICAL_ERROR("bad band number");
			return 1;
		}

		memcpy(&handle->band_set_params[band_num], p_band_set_params,
			sizeof(struct biquad_filter_api_band_set_params_t));
		curr_coeffs = &handle->biquad_bands_coeffs[5 * band_num];
		filter_mode = p_band_set_params->filter_mode;
		if (0 != p_band_set_params->bypass)
		{
			filter_mode = BIQUADS_TRANSPARENT_MODE;
		}
		biquads_coefficients_calculation(filter_mode, p_band_set_params->Fc,
				p_band_set_params->QValue,	p_band_set_params->Gain_db,
				p_band_set_params->Sample_rate, curr_coeffs);
		break;

	case IOCTL_BIQUAD_FILTER_SET_BAND_RAW_COEFFICIENTS :
		band_raw_params =
				((struct  biquad_filter_api_band_set_raw_t*)aIoctl_param1);
		num_of_bands = handle->num_of_bands;
		band_num = band_raw_params->band_num ;
		p_band_set_coefficients = &(band_raw_params->band_set_coefficients);

		if ((num_of_bands <= band_num ))
		{
			CRITICAL_ERROR("bad band number");
			return 1;
		}

		curr_coeffs = &handle->biquad_bands_coeffs[5 * band_num];
		biquads_coefficients_assign(p_band_set_coefficients->b0,
				p_band_set_coefficients->b1,
				p_band_set_coefficients->b2,
				p_band_set_coefficients->a1,
				p_band_set_coefficients->a2,
				curr_coeffs);

		break;

	case IOCTL_BIQUAD_FILTER_GET_BAND :
		band_params = ((struct biquad_filter_api_band_set_t*)aIoctl_param1);
		p_band_set_params = &(band_params->band_set_params);
		num_of_bands = handle->num_of_bands;
		band_num = band_params->band_num ;
		if (num_of_bands > band_num )
		{
			memcpy(p_band_set_params,
					&handle->band_set_params[band_num],
					sizeof(struct biquad_filter_api_band_set_params_t));
		}
		break;

	default :
		return 1;
	}
	return 0;
}


/**
 * biquad_filter_init()
 *
 * return:
 */
extern "C" void  biquad_filter_init(void)
{
	DSP_REGISTER_NEW_MODULE("biquad_filter",
			biquad_filter_ioctl, biquad_filter_dsp,
			dsp_management_default_bypass, dsp_management_default_mute,
			0, struct biquads_filter_t);
}

AUTO_INIT_FUNCTION(biquad_filter_init);
