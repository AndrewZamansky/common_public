/*
 *
 * file :   equalizer.c
 *
 *
 *
 *
 *
 */



/********  includes *********************/

#include "_equalizer_prerequirements_check.h"

#include "PRINTF_api.h"

#include "equalizer_api.h" //place first to test that header file is self-contained
#include "equalizer.h"

#include "auto_init_api.h"

/********  defines *********************/


/********  types  *********************/

/********  externals *********************/


/********  exported variables *********************/

char equalizer_module_name[] = "equalizer";


/**********   external variables    **************/



/***********   local variables    **************/



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        equalizer_dsp                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
void equalizer_dsp(pdsp_descriptor apdsp , size_t data_len ,
		dsp_pad_t *in_pads[MAX_NUM_OF_OUTPUT_PADS] , dsp_pad_t out_pads[MAX_NUM_OF_OUTPUT_PADS])
{
	float *apCh1In  ;
	float *apCh1Out ;
	EQUALIZER_Instance_t *handle;

	handle = apdsp->handle;

	if(0 == handle->num_of_bands)
	{
		return;
	}

	apCh1In = in_pads[0]->buff;
	apCh1Out = out_pads[0].buff;

	biquads_cascading_filter(handle->pBiquadFilter , apCh1In , apCh1Out , data_len);

}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        equalizer_ioctl                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t equalizer_ioctl(pdsp_descriptor apdsp ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2)
{
	uint8_t i;
	size_t num_of_bands;
	uint8_t band_num;
	BandCoeffs_t *pCoeffs;
	equalizer_api_band_set_params_t *p_band_set_params;
	EQUALIZER_Instance_t *handle;

	handle = apdsp->handle;
	switch(aIoctl_num)
	{
		case IOCTL_DSP_INIT :
			handle->num_of_bands =0;
			handle->pCoeffs = NULL ;

			break;

		case IOCTL_EQUALIZER_SET_NUM_OF_BANDS :
			num_of_bands = ((size_t)aIoctl_param1);
			handle->num_of_bands = num_of_bands;
			free(handle->pCoeffs);

			pCoeffs=(BandCoeffs_t *)malloc(sizeof(BandCoeffs_t) * num_of_bands);
			handle->pCoeffs = pCoeffs;
			for(i=0 ; i<num_of_bands ; i++)
			{
				pCoeffs[i].a1 = 0;
				pCoeffs[i].a2 = 0;
				pCoeffs[i].b0 = 1;
				pCoeffs[i].b1 = 0;
				pCoeffs[i].b2 = 0;
			}

			handle->pBiquadFilter = biquads_alloc(handle->num_of_bands , (float *)pCoeffs );

			break;

		case IOCTL_EQUALIZER_SET_BAND_BIQUADS :
			num_of_bands = handle->num_of_bands;
			band_num = ((equalizer_api_band_set_t*)aIoctl_param1)->band_num ;
			if(num_of_bands > band_num )
			{
				p_band_set_params = &(((equalizer_api_band_set_t*)aIoctl_param1)->band_set_params);
				memcpy(&handle->band_set_params,
						p_band_set_params,sizeof(equalizer_api_band_set_params_t));
				pCoeffs = &handle->pCoeffs[band_num];
				biquads_calculation(
						p_band_set_params->filter_mode,
						p_band_set_params->Fc,
						p_band_set_params->QValue,
						p_band_set_params->Gain,
						48000,
						(float*)pCoeffs
						);
			}
			break;

		case IOCTL_EQUALIZER_GET_BAND_BIQUADS :
			p_band_set_params = &(((equalizer_api_band_set_t*)aIoctl_param1)->band_set_params);
			memcpy(p_band_set_params,
					&handle->band_set_params, sizeof(equalizer_api_band_set_params_t));
			break;

		default :
			return 1;
	}
	return 0;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:         equalizer_init                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
void  equalizer_init(void)
{
	DSP_REGISTER_NEW_MODULE("equalizer",equalizer_ioctl , equalizer_dsp , EQUALIZER_Instance_t);
}

AUTO_INIT_FUNCTION(equalizer_init);
