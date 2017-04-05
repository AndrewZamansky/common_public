/*
 *
 * file :   standard_compressor.c
 *
 *
 *
 *
 *
 */



/********  includes *********************/

#include "_standard_compressor_prerequirements_check.h" // should be after {standard_compressor_config.h,dev_management_api.h}

#include "standard_compressor_api.h" //place first to test that header file is self-contained
#include "standard_compressor.h"

#include "math.h"

#include "PRINTF_api.h"


#include "auto_init_api.h"

/********  defines *********************/

/********  types  *********************/

/********  externals *********************/

/********  exported variables *********************/

char standard_compressor_module_name[] = "standard_compressor";


/**********   external variables    **************/

/***********   local variables    **************/
#define ALPHA				0.96f

/* division by 4 put here instead of division by 2 in mono->stereo converter*/
#define ONE_MINUS_ALPHA		((1.0f - ALPHA)/4)


/**
 * standard_compressor_dsp()
 *
 * return:
 */
void standard_compressor_dsp(struct dsp_desc_t *adsp , size_t data_len ,
		struct dsp_pad_t *in_pads[MAX_NUM_OF_OUTPUT_PADS] ,
		struct dsp_pad_t out_pads[MAX_NUM_OF_OUTPUT_PADS])
{

	STANDARD_COMPRESSOR_Instance_t *handle;
	float *apCh1In ,  *apCh2In;
	float *apCh1Out ,  *apCh2Out;

	float env_follower ;
	float rms ;
	float curr_ratio = 1;
	float threshold  ;
	float reverse_ratio ;
	float attack ;
	float release ;
	float release_neg ;
	float attack_neg ;
	float curr_x1, curr_x2;
	float tmp;
	float mono_x;
	float gain;
	float alpha , one_minus_alpha;

	handle = adsp->handle;
	apCh1In = in_pads[0]->buff;
	apCh2In = in_pads[1]->buff;
	apCh1Out = out_pads[0].buff;
	apCh2Out = out_pads[1].buff;

	attack = handle->attack;
	attack_neg = 1 - attack;
	release = handle->release;
	release_neg =  1 - release ;
	threshold = handle->threshold;
	reverse_ratio = handle->reverse_ratio;
	gain = handle->gain;

	env_follower = handle->env_follower;
	rms = handle->rms;

	alpha = handle->alpha;
	one_minus_alpha = handle->one_minus_alpha;

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

		rms = fast_pow(rms , 0.5);

		curr_ratio = 1;
		if(rms > threshold)
		{
			curr_ratio = threshold/rms ;
			tmp = fast_pow(curr_ratio , reverse_ratio);
			curr_ratio = curr_ratio / tmp;
		}

		
		if (curr_ratio < env_follower)
		{
			curr_ratio *=attack;
			env_follower *= attack_neg;
		}
		else
		{
			curr_ratio *=release;
			env_follower *= release_neg;
		}
		env_follower += curr_ratio;


		curr_x1 *=gain;
		*apCh1Out++ = (env_follower * curr_x1);
		curr_x2 *=gain;
		*apCh2Out++ = (env_follower * curr_x2);
	}

	handle->env_follower = env_follower;
	handle->rms = rms;

}


/**
 * standard_compressor_ioctl()
 *
 * return:
 */
uint8_t standard_compressor_ioctl(struct dsp_desc_t *adsp,
		uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2)
{
	STANDARD_COMPRESSOR_Instance_t *handle;

	handle = adsp->handle;
	switch(aIoctl_num)
	{
		case IOCTL_DSP_INIT :
			handle->reverse_ratio = 1.0f;//0.5;
			handle->env_follower = 1.0f;
			handle->rms = 0.0f;
			handle->release = 16.0f;
			handle->attack = 1.0f;
			handle->threshold = 0.99999f;
			handle->gain =1.0f;
			handle->alpha = ALPHA;
			handle->one_minus_alpha = ONE_MINUS_ALPHA;

			break;
		case IOCTL_STANDARD_COMPRESSOR_SET_HIGH_THRESHOLD :
			handle->threshold = *((float*)aIoctl_param1);
			break;
		case IOCTL_STANDARD_COMPRESSOR_SET_RATIO :
			handle->reverse_ratio = 1/(*((float*)aIoctl_param1));
			break;
		case IOCTL_STANDARD_COMPRESSOR_SET_ATTACK :
			handle->attack = *((float*)aIoctl_param1);
			break;
		case IOCTL_STANDARD_COMPRESSOR_SET_RELEASE :
			handle->release = *((float*)aIoctl_param1);
			break;
		default :
			return 1;
	}
	return 0;
}




void  standard_compressor_init(void)
{
	DSP_REGISTER_NEW_MODULE(STANDARD_COMPRESSOR_API_MODULE_NAME,
			standard_compressor_ioctl, standard_compressor_dsp,
			STANDARD_COMPRESSOR_Instance_t);
}

AUTO_INIT_FUNCTION(standard_compressor_init);

