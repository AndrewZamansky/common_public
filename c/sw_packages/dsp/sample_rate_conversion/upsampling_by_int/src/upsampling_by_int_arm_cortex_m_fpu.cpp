/*
 *
 * file :   upsampling_by_int_arm_cortex_m.c
 *
 *
 */


#include "_project_typedefs.h"
#include "_project_defines.h"

#include "dsp_management_api.h"
#include "common_dsp_api.h"

#include "cpu_config.h"
#include "arm_math.h"
#include "upsampling_by_int.h"
#include "upsampling_by_int_api.h"


struct upsampling_by_int_t {
	void *p_filter_instance;
	float *p_filter_state;
};



void upsampling_by_int_function(
		void *pFilter, float *apIn, float *apOut, size_t buff_len)
{
	arm_fir_interpolate_instance_f32 *filter_params;
	struct upsampling_by_int_t * upsampling_by_int;

	upsampling_by_int = (struct upsampling_by_int_t *)upsampling_by_int;
	filter_params = (arm_fir_interpolate_instance_f32*)(
									upsampling_by_int->p_filter_instance);
	arm_fir_interpolate_f32(filter_params, apIn, apOut, buff_len );
}




/*   func : fir_alloc()
 *    The coefficients are stored in the array pCoeffs in the following order:
 *
 *    y[n] = b[0] * x[n] + b[1] * x[n-1] + b[2] * x[n-2] + ...+ b[numTaps-1] * x[n-numTaps+1]
 *
 *    {b[numTaps-1], b[numTaps-2], b[N-2], ..., b[1], b[0]}
 */
void *upsampling_by_int_alloc(size_t factor,
		size_t number_of_filter_coefficients,
		float *p_coefficients ,	size_t predefined_data_block_size)
{
	arm_status	status;
	struct upsampling_by_int_t *p_upsampling_by_int;
	arm_fir_interpolate_instance_f32* p_filter_instance;
	float *p_filter_state;

	p_upsampling_by_int=(struct upsampling_by_int_t *)calloc(
									1, sizeof(struct upsampling_by_int_t));

	p_filter_instance = calloc(1,sizeof( arm_fir_interpolate_instance_f32));
	p_upsampling_by_int->p_filter_instance = p_filter_instance;

	p_filter_state = (float*)calloc(
			((number_of_filter_coefficients / factor) +
					predefined_data_block_size - 1) , sizeof(float));
	p_upsampling_by_int->p_filter_state = p_filter_state;

	status = arm_fir_interpolate_init_f32 (p_filter_instance ,
			(uint8_t)factor, (uint16_t)number_of_filter_coefficients ,
			p_coefficients, p_filter_state, predefined_data_block_size);

	if (ARM_MATH_SUCCESS != status)
	{
		upsampling_by_int_free(p_upsampling_by_int);
		p_upsampling_by_int = NULL;
	}
	return p_upsampling_by_int;
}


/*  func : firs_free()
 */
void upsampling_by_int_free(void *pFilter)
{
	os_safe_free(((struct upsampling_by_int_t *)pFilter)->p_filter_state);
	os_safe_free(((struct upsampling_by_int_t *)pFilter)->p_filter_instance);
	os_safe_free(pFilter);
}
