/*
 *
 * file :   multiplier_1ch.c
 *
 *
 */



/********  includes *********************/
#include "_project_typedefs.h"
#include "_project_defines.h"

#include "dsp_management_api.h"
#include "dsp_management_internal_api.h"

#include "multiplier_api.h"

#include "multiplier_1ch_api.h"
#include "multiplier_1ch.h"

#include "auto_init_api.h"

#include "string.h"
#include "math.h"

#include "_multiplier_1ch_prerequirements_check.h"

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
void multiplier_1ch_dsp(struct dsp_desc_t *adsp,
		struct dsp_pad_t *in_pads[MAX_NUM_OF_OUTPUT_PADS],
		struct dsp_pad_t  out_pads[MAX_NUM_OF_OUTPUT_PADS])
{
	real_t *apCh1In ;
	real_t *apCh1Out  ;
	struct multiplier_1ch_instance_t *handle;
	real_t weight ;
	real_t curr_val;
	size_t in_data_len ;
	size_t out_data_len ;

	handle = (struct multiplier_1ch_instance_t *)adsp->handle;

	weight = handle->weight;

	DSP_GET_BUFFER(in_pads[0], &apCh1In, &in_data_len);
	DSP_GET_BUFFER(&out_pads[0], &apCh1Out, &out_data_len);

	if (in_data_len > out_data_len )
	{
		CRITICAL_ERROR("bad buffers sizes");
	}

	while (in_data_len--)
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
	real_t weight;
	real_t tmp;
	struct multiplier_1ch_instance_t *handle;
	struct multiplier_api_set_params_t *set_params;

	handle = (struct multiplier_1ch_instance_t *)adsp->handle;
	set_params = &handle->set_params;

	switch(aIoctl_num)
	{
	case IOCTL_DSP_INIT :
		handle->weight = (int16_t)1;
		set_params->weight = (int16_t)0;
		break;

	case IOCTL_MULTIPLIER_SET_WEIGHT :
		weight = *(float*)aIoctl_param1;
		tmp = (float)10;
		#ifdef CONFIG_DSP_REAL_NUMBER_FORMAT_FLOATING_POINT
			handle->weight = pow(tmp, weight/(float)20);
		#else
			tmp = fix16_log(tmp);
			tmp *= (weight / (float)20);
			handle->weight = fix16_exp(tmp);
		#endif
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
