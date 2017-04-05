/*
 *
 * file :   fir_filter.c
 *
 *
 *
 *
 *
 */



/********  includes *********************/

#include "_fir_filter_prerequirements_check.h"

#include "PRINTF_api.h"

#include "fir_filter_api.h" //place first to test that header file is self-contained
#include "fir_filter.h"

#include "auto_init_api.h"
#include "math.h"

/********  defines *********************/


/********  types  *********************/

/********  externals *********************/


/********  exported variables *********************/

char fir_filter_module_name[] = "fir_filter";


/**********   external variables    **************/



/***********   local variables    **************/



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        fir_filter_dsp                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
static void fir_filter_dsp(pdsp_descriptor apdsp , size_t data_len ,
		dsp_pad_t *in_pads[MAX_NUM_OF_OUTPUT_PADS] , dsp_pad_t out_pads[MAX_NUM_OF_OUTPUT_PADS])
{
	float *apCh1In  ;
	float *apCh1Out ;
	FIR_FILTER_Instance_t *handle;

	handle = apdsp->handle;

	if(0 == handle->number_of_filter_coefficients)
	{
		return;
	}

	apCh1In = in_pads[0]->buff;
	apCh1Out = out_pads[0].buff;

	fir_filter_function(handle->p_fir_filter , apCh1In , apCh1Out , data_len);

}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        fir_filter_ioctl                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t fir_filter_ioctl(pdsp_descriptor apdsp ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2)
{
	size_t number_of_filter_coefficients;
	size_t predefined_data_block_size;
	float *p_coefficients;
	fir_filter_api_set_params_t *p_band_set_params;
	FIR_FILTER_Instance_t *handle;

	handle = apdsp->handle;
	switch(aIoctl_num)
	{
		case IOCTL_DSP_INIT :
			handle->number_of_filter_coefficients =0;
			handle->p_coefficients = NULL ;

			break;


		case IOCTL_FIR_FILTER_SET_FIR_COEFFICIENTS :
			p_band_set_params = ((fir_filter_api_set_params_t *)aIoctl_param1);
			number_of_filter_coefficients = p_band_set_params->number_of_filter_coefficients;
			handle->number_of_filter_coefficients = number_of_filter_coefficients;
			predefined_data_block_size = p_band_set_params->predefined_data_block_size;
			handle->predefined_data_block_size = predefined_data_block_size;

			p_coefficients=(float *)realloc(handle->p_coefficients, sizeof(float) * number_of_filter_coefficients);
			memcpy(p_coefficients,p_band_set_params->p_coefficients , sizeof(float)*number_of_filter_coefficients);
			handle->p_coefficients = p_coefficients;

			handle->p_fir_filter = fir_alloc(number_of_filter_coefficients , p_coefficients ,predefined_data_block_size );
			break;

		default :
			return 1;
	}
	return 0;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:         fir_filter_init                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
void  fir_filter_init(void)
{
	DSP_REGISTER_NEW_MODULE("fir_filter",fir_filter_ioctl , fir_filter_dsp , FIR_FILTER_Instance_t);
}

AUTO_INIT_FUNCTION(fir_filter_init);