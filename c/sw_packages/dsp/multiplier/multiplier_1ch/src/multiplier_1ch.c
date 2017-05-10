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


/**
 * multiplier_1ch_dsp()
 *
 * return:
 */
void multiplier_1ch_dsp(struct dsp_desc_t *adsp , size_t data_len ,
		struct dsp_pad_t *in_pads[MAX_NUM_OF_OUTPUT_PADS],
		struct dsp_pad_t  out_pads[MAX_NUM_OF_OUTPUT_PADS])
{
	float *apCh1In ;
	float *apCh1Out  ;
	struct multiplier_1ch_instance_t *handle;
	float weight ;
	float curr_val;

	handle = adsp->handle;

	weight = handle->weight;

	apCh1In = in_pads[0]->buff;
	apCh1Out = out_pads[0].buff;


	for( ; data_len ;data_len--)
	{
		curr_val = (*apCh1In++) * weight;
		*apCh1Out++ = curr_val;
	}

}





/**
 * multiplier_1ch_ioctl()
 *
 * return:
 */
uint8_t multiplier_1ch_ioctl(struct dsp_desc_t *adsp,
		const uint8_t aIoctl_num, void * aIoctl_param1 , void * aIoctl_param2)
{
	float weight;
	struct multiplier_1ch_instance_t *handle;
	struct multiplier_api_set_params_t *set_params;

	handle = adsp->handle;
	set_params = &handle->set_params;

	switch(aIoctl_num)
	{
	case IOCTL_DSP_INIT :
		handle->weight = 1;
		set_params->weight = 0;
		break;

	case IOCTL_MULTIPLIER_SET_WEIGHT :
		weight = *(float*)aIoctl_param1;
		handle->weight = = pow(10, weight / 20.0f);;
		set_params->weight = weight;
		break;

	case IOCTL_MULTIPLIER_GET_PARAMS :
		memcpy(aIoctl_param1, set_params,
				sizeof(struct multiplier_api_set_params_t));
		break;
	default :
			return 1;
	}
	return 0;
}



/**
 * multiplier_1ch_init()
 *
 * return:
 */
void  multiplier_1ch_init(void)
{
	DSP_REGISTER_NEW_MODULE("multiplier_1ch", multiplier_1ch_ioctl,
			multiplier_1ch_dsp , struct multiplier_1ch_instance_t);
}

AUTO_INIT_FUNCTION(multiplier_1ch_init);
