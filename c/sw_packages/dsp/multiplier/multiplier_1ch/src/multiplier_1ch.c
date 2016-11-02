/*
 *
 * file :   multiplier_1ch.c
 *
 *
 *
 *
 *
 */



/********  includes *********************/

#include "_multiplier_1ch_prerequirements_check.h"

#include "multiplier_1ch_api.h" //place first to test that header file is self-contained
#include "multiplier_1ch.h"
#include "common_dsp_api.h"

#include "auto_init_api.h"

/********  defines *********************/

/********  types  *********************/

/********  externals *********************/


/********  exported variables *********************/

char multiplier_1ch_module_name[] = "multiplier_1ch";


/**********   external variables    **************/



/***********   local variables    **************/

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        multiplier_1ch_dsp                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
void multiplier_1ch_dsp(pdsp_descriptor apdsp , size_t data_len ,
		dsp_pad_t *in_pads[MAX_NUM_OF_OUTPUT_PADS] , dsp_pad_t  out_pads[MAX_NUM_OF_OUTPUT_PADS])
{
	float *apCh1In ;
	float *apCh1Out  ;
	MULTIPLIER_1CH_Instance_t *handle;
	float weight ;
	float curr_val;

	handle = apdsp->handle;

	weight = handle->weight;

	apCh1In = in_pads[0]->buff;
	apCh1Out = out_pads[0].buff;


	for( ; data_len ;data_len--)
	{
		curr_val = (*apCh1In++) * weight;
		*apCh1Out++ = curr_val;
	}

}





/*---------------------------------------------------------------------------------------------------------*/
/* Function:        multiplier_1ch_ioctl                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t multiplier_1ch_ioctl(pdsp_descriptor apdsp ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2)
{
	MULTIPLIER_1CH_Instance_t *handle;

	handle = apdsp->handle;

	switch(aIoctl_num)
	{
		case IOCTL_DSP_INIT :
			handle->weight = 0;
			break;

		case IOCTL_MULTIPLIER_SET_WEIGHT :
			handle->weight = *(float*)aIoctl_param1;
			break;


		default :
			return 1;
	}
	return 0;
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:         multiplier_1ch_init                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
void  multiplier_1ch_init(void)
{
	DSP_REGISTER_NEW_MODULE("multiplier_1ch",multiplier_1ch_ioctl , multiplier_1ch_dsp , MULTIPLIER_1CH_Instance_t);
}

AUTO_INIT_FUNCTION(multiplier_1ch_init);
