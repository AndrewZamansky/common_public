/*
 *
 * file :   upsampling_by_int_filter.c
 *
 *
 *
 *
 *
 */



/********  includes *********************/

#include "_upsampling_by_int_prerequirements_check.h"

#include "PRINTF_api.h"

#include "upsampling_by_int_api.h" //place upsampling_by_intst to test that header file is self-contained
#include "upsampling_by_int.h"

#include "auto_init_api.h"
#include "math.h"

/********  defines *********************/


/********  types  *********************/

/********  externals *********************/


/********  exported variables *********************/

char upsampling_by_int_module_name[] = "upsampling_by_int";


/**********   external variables    **************/



/***********   local variables    **************/



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        upsampling_by_int_filter_dsp                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
static void upsampling_by_int_dsp(pdsp_descriptor apdsp , size_t data_len ,
		dsp_pad_t *in_pads[MAX_NUM_OF_OUTPUT_PADS] , dsp_pad_t out_pads[MAX_NUM_OF_OUTPUT_PADS])
{
	float *apCh1In  ;
	float *apCh1Out ;
	UPSAMPLING_BY_INT_Instance_t *handle;

	handle = apdsp->handle;

	if((0 == handle->number_of_filter_coefficients)
		|| (0 == handle->factor) || (NULL == handle->p_upsampling_by_int_filter))
	{
		return;
	}

	apCh1In = in_pads[0]->buff;
	apCh1Out = out_pads[0].buff;
	data_len = data_len/handle->factor; // should be removed later ???
	upsampling_by_int_function(handle->p_upsampling_by_int_filter , apCh1In , apCh1Out , data_len);

}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        upsampling_by_int_ioctl                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t upsampling_by_int_ioctl(pdsp_descriptor apdsp ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2)
{
	size_t number_of_filter_coefficients;
	size_t predefined_data_block_size;
	float *p_coefficients;
	upsampling_by_int_api_set_params_t *p_band_set_params;
	UPSAMPLING_BY_INT_Instance_t *handle;

	handle = apdsp->handle;
	switch(aIoctl_num)
	{
		case IOCTL_DSP_INIT :
			handle->factor =0;
			handle->number_of_filter_coefficients =0;
			handle->p_coefficients = NULL ;
			handle->p_upsampling_by_int_filter = NULL;

			break;

		case IOCTL_UPSAMPLING_BY_INT_SET_FACTOR :
			handle->factor = *(size_t*)aIoctl_param1;

			break;

		case IOCTL_UPSAMPLING_BY_INT_SET_FIR_COEFFICIENTS :
			p_band_set_params = ((upsampling_by_int_api_set_params_t *)aIoctl_param1);
			number_of_filter_coefficients = p_band_set_params->number_of_filter_coefficients;
			handle->number_of_filter_coefficients = number_of_filter_coefficients;
			predefined_data_block_size = p_band_set_params->predefined_data_block_size;
			handle->predefined_data_block_size = predefined_data_block_size;

			p_coefficients=(float *)realloc(handle->p_coefficients, sizeof(float) * number_of_filter_coefficients);
			memcpy(p_coefficients,p_band_set_params->p_coefficients , sizeof(float)*number_of_filter_coefficients);
			handle->p_coefficients = p_coefficients;

			handle->p_upsampling_by_int_filter = upsampling_by_int_alloc(handle->factor ,
					number_of_filter_coefficients , p_coefficients ,predefined_data_block_size );
			break;

		default :
			return 1;
	}
	return 0;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:         upsampling_by_int_init                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
void  upsampling_by_int_init(void)
{
	DSP_REGISTER_NEW_MODULE("upsampling_by_int",upsampling_by_int_ioctl , upsampling_by_int_dsp , UPSAMPLING_BY_INT_Instance_t);
}

AUTO_INIT_FUNCTION(upsampling_by_int_init);