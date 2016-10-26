/*
 *
 * file :   downsampling_by_int_arm_cortex_m.c
 *
 *
 *
 *
 *
 */



/********  includes *********************/

#include "_project.h"
#include "_downsampling_by_int_prerequirements_check.h"
#include "common_dsp_api.h"
#include "cpu_config.h"
#include "arm_math.h"
#include "downsampling_by_int.h"
#include "downsampling_by_int_api.h"


/********  defines *********************/


/********  types  *********************/

typedef struct
{
	void *p_filter_instance;
	float *p_filter_state;
}downsampling_by_int_t;



/********  externals *********************/


/********  local defs *********************/


/**********   external variables    **************/



/***********   local variables    **************/

void downsampling_by_int_function(void *pFilter,float *apIn,float *apOut,size_t buff_len)
{
	arm_fir_decimate_instance_f32 *filter_params;
	filter_params = ((arm_fir_decimate_instance_f32*)(((downsampling_by_int_t *)pFilter)->p_filter_instance));
	arm_fir_decimate_f32(filter_params,apIn,apOut ,buff_len );
}




/*   func : fir_alloc()
 *    The coefficients are stored in the array pCoeffs in the following order:
 *
 *    y[n] = b[0] * x[n] + b[1] * x[n-1] + b[2] * x[n-2] + ...+ b[numTaps-1] * x[n-numTaps+1]
 *
 *    {b[numTaps-1], b[numTaps-2], b[N-2], ..., b[1], b[0]}
 */
void *downsampling_by_int_alloc(size_t factor ,size_t number_of_filter_coefficients,
		float *p_coefficients ,	size_t predefined_data_block_size)
{
	arm_status	status;
	downsampling_by_int_t *p_downsampling_by_int;
	arm_fir_decimate_instance_f32* p_filter_instance;
	float *p_filter_state;

	p_downsampling_by_int=(downsampling_by_int_t *)calloc(1,sizeof(downsampling_by_int_t));

	p_filter_instance = calloc(1,sizeof( arm_fir_decimate_instance_f32));
	p_downsampling_by_int->p_filter_instance = p_filter_instance;

	p_filter_state = (float*)calloc( (number_of_filter_coefficients  + predefined_data_block_size - 1), sizeof(float));
	p_downsampling_by_int->p_filter_state = p_filter_state;

	status = arm_fir_decimate_init_f32 (p_filter_instance , (uint16_t)number_of_filter_coefficients ,(uint8_t)factor,
			p_coefficients, p_filter_state, predefined_data_block_size);

	if (ARM_MATH_SUCCESS != status)
	{
		downsampling_by_int_free(p_downsampling_by_int);
		p_downsampling_by_int = NULL;
	}
	return p_downsampling_by_int;
}


/*  func : firs_free()
 */
void downsampling_by_int_free(void *pFilter)
{
	free(((downsampling_by_int_t *)pFilter)->p_filter_state);
	free(((downsampling_by_int_t *)pFilter)->p_filter_instance);
	free(pFilter);
}
