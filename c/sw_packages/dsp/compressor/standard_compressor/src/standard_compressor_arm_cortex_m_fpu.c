/*
 *
 * file :   standard_compressor.c
 *
 *
 */



/********  includes *********************/
#include "_project_typedefs.h"
#include "_project_defines.h"

#include "dsp_management_api.h"
#include "common_dsp_api.h"


#include "standard_compressor_api.h"
#include "standard_compressor.h"

#include "math.h"
#include "cpu_config.h"
#include "arm_math.h"

#include "auto_init_api.h"

#include "_standard_compressor_prerequirements_check.h"

/********  defines *********************/

/********  types  *********************/

/********  externals *********************/

/********  exported variables *********************/

/**********   external variables    **************/

/***********   local variables    **************/


/**
 * standard_compressor_dsp()
 *
 * return:
 */
void standard_compressor_dsp(struct dsp_desc_t *adsp , size_t data_len ,
		struct dsp_pad_t *in_pads[MAX_NUM_OF_OUTPUT_PADS] ,
		struct dsp_pad_t out_pads[MAX_NUM_OF_OUTPUT_PADS])
{

	struct std_compressor_instance_t *handle;
	float const *apCh1In;
	float const *apCh2In;
	float *apCh1Out ,  *apCh2Out;

	static float ratio_env_follower ;
	float rms ;
	float curr_ratio = 1;
	float threshold  ;
	float reverse_ratio ;
	float attack ;
	float release ;
	static float release_neg ;
	float attack_neg ;
	float curr_x1, curr_x2;
	float tmp;
	float mono_x;
	float gain;
	float alpha , one_minus_alpha;//

	handle = adsp->handle;
	apCh1In = in_pads[0]->buff;
	apCh2In = in_pads[1]->buff;
	apCh1Out = out_pads[0].buff;
	apCh2Out = out_pads[1].buff;

	attack = handle->attack;
	attack_neg = 1.0f - attack;
	release = handle->release;
	release_neg =  1.0f - release ;
	threshold = handle->threshold;
	reverse_ratio = handle->reverse_ratio;
	gain = handle->gain;

	ratio_env_follower = handle->ratio_env_follower;
	rms = handle->rms;

	alpha = handle->alpha;
	one_minus_alpha = handle->one_minus_alpha;

	//  following function take more CPU than straight forward calculations :
	//	arm_add_f32 (apCh1In, apCh2In, apCh1Out, data_len);
	//	arm_mult_f32 (apCh1Out, apCh1Out, apCh2Out, data_len);
	//	arm_scale_f32 (apCh2Out, one_minus_alpha, apCh1Out, data_len);

	while( data_len-- )
	{

		curr_x1 = *apCh1In++;
		curr_x2 = *apCh2In++;

		mono_x = curr_x1 + curr_x2;
	  /*mono_x = mono_x/2 ;*/ /* division by 2 is inserted in ONE_MINUS_ALPHA*/

		mono_x = mono_x * mono_x;
		mono_x *= one_minus_alpha ;
		rms = rms*rms;
		rms *= alpha;
		rms +=mono_x;

		arm_sqrt_f32(rms , &rms);

		if(rms > threshold)
		{
			curr_ratio = threshold/rms ;
			tmp = fast_pow(curr_ratio , reverse_ratio);
			curr_ratio = curr_ratio / tmp;
		}
		else
		{
			curr_ratio = 1;
		}


		if (curr_ratio < ratio_env_follower)
		{
			curr_ratio = fast_pow(curr_ratio, attack);
			ratio_env_follower = fast_pow(ratio_env_follower, attack_neg);
		}
		else
		{
			curr_ratio = fast_pow(curr_ratio, release);
			ratio_env_follower = fast_pow(ratio_env_follower, release_neg);
		}
		ratio_env_follower *= curr_ratio;


		curr_x1 *= gain;
		*apCh1Out++ = (ratio_env_follower * curr_x1);
		curr_x2 *= gain;
		*apCh2Out++ = (ratio_env_follower * curr_x2);
	}

	//  following function take more CPU than straight forward calculations :
	// arm_scale_f32 (apCh1Out, gain, apCh1Out, data_len);
	// arm_scale_f32 (apCh2Out, gain, apCh2Out, data_len);

	handle->ratio_env_follower = ratio_env_follower;
	handle->rms = rms;

}
