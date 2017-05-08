/*
 *
 * file :   biquad_filter.c
 *
 *
 */



/********  includes *********************/

#include "_biquad_filter_prerequirements_check.h"

#include "PRINTF_api.h"

#include "biquad_filter_api.h"
#include "biquad_filter.h"

#include "auto_init_api.h"

/********  defines *********************/


/********  types  *********************/

/********  externals *********************/


/********  exported variables *********************/

char biquad_filter_module_name[] = "biquad_filter";


/**********   external variables    **************/



/***********   local variables    **************/



/**
 * biquad_filter_dsp()
 *
 * return:
 */
void biquad_filter_dsp(struct dsp_desc_t *adsp, size_t data_len,
		struct dsp_pad_t *in_pads[MAX_NUM_OF_OUTPUT_PADS],
		struct dsp_pad_t out_pads[MAX_NUM_OF_OUTPUT_PADS])
{
	float *apCh1In  ;
	float *apCh1Out ;
	struct biquads_filter_t *handle;

	handle = adsp->handle;

	if(0 == handle->num_of_bands)
	{
		return;
	}

	apCh1In = in_pads[0]->buff;
	apCh1Out = out_pads[0].buff;

	biquads_cascading_filter(handle->pBiquadFilter,
			apCh1In, apCh1Out, data_len);

}


/**
 * biquad_filter_ioctl()
 *
 * return:
 */
uint8_t biquad_filter_ioctl(struct dsp_desc_t *adsp,
		const uint8_t aIoctl_num, void * aIoctl_param1 , void * aIoctl_param2)
{
	uint8_t i;
	size_t num_of_bands;
	uint8_t band_num;
	struct biquad_band_coeffs_t *pCoeffs;
	biquad_filter_api_band_set_params_t *p_band_set_params;
	struct biquads_filter_t *handle;
	biquad_filter_api_band_set_t *band_params;

	handle = adsp->handle;
	switch(aIoctl_num)
	{
	case IOCTL_DSP_INIT :
		handle->num_of_bands =0;
		handle->pCoeffs = NULL ;

		break;

	case IOCTL_BIQUAD_FILTER_SET_NUM_OF_BANDS :
		num_of_bands = ((size_t)aIoctl_param1);
		handle->num_of_bands = num_of_bands;
		free(handle->pCoeffs);

		pCoeffs = (struct biquad_band_coeffs_t *)malloc(
				sizeof(struct biquad_band_coeffs_t) * num_of_bands);

		handle->pCoeffs = pCoeffs;
		for (i=0; i<num_of_bands; i++)
		{
			pCoeffs[i].a1 = 0;
			pCoeffs[i].a2 = 0;
			pCoeffs[i].b0 = 1;
			pCoeffs[i].b1 = 0;
			pCoeffs[i].b2 = 0;
		}

		handle->pBiquadFilter =
				biquads_alloc(handle->num_of_bands, (float *)pCoeffs );

		break;

	case IOCTL_BIQUAD_FILTER_SET_BAND_BIQUADS :
		band_params = ((biquad_filter_api_band_set_t*)aIoctl_param1);
		num_of_bands = handle->num_of_bands;
		band_num = band_params->band_num ;
		p_band_set_params = &(band_params->band_set_params);
		if ((num_of_bands > band_num ) && (p_band_set_params->Fc > 0.01))
		{
			memcpy(&handle->band_set_params,
				p_band_set_params, sizeof(biquad_filter_api_band_set_params_t));
			pCoeffs = &handle->pCoeffs[band_num];
			biquads_coefficients_calculation(
					p_band_set_params->filter_mode,
					p_band_set_params->Fc,
					p_band_set_params->QValue,
					p_band_set_params->Gain,
					48000,
					(float*)pCoeffs
					);
		}
		break;

	case IOCTL_BIQUAD_FILTER_GET_BAND_BIQUADS :
		band_params = ((biquad_filter_api_band_set_t*)aIoctl_param1);
		p_band_set_params = &(band_params->band_set_params);
		memcpy(p_band_set_params,
				&handle->band_set_params,
				sizeof(biquad_filter_api_band_set_params_t));
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
void  biquad_filter_init(void)
{
	DSP_REGISTER_NEW_MODULE("biquad_filter",
			biquad_filter_ioctl, biquad_filter_dsp, struct biquads_filter_t);
}

AUTO_INIT_FUNCTION(biquad_filter_init);
