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

#ifdef CONFIG_USE_HW_DSP
  #include "cpu_config.h"
  #include "arm_math.h"
#endif

#include "auto_init_api.h"

/********  defines *********************/

/********  types  *********************/

/********  externals *********************/

/********  exported variables *********************/

char standard_compressor_module_name[] = "standard_compressor";


/**********   external variables    **************/

/***********   local variables    **************/


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        standard_compressor_dsp                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
void standard_compressor_dsp(pdsp_descriptor apdsp , size_t data_len ,
		dsp_pad_t *in_pads[MAX_NUM_OF_OUTPUT_PADS] , dsp_pad_t out_pads[MAX_NUM_OF_OUTPUT_PADS])
{

	STANDARD_COMPRESSOR_Instance_t *handle;
	float *apCh1In ,  *apCh2In;
	float *apCh1Out ,  *apCh2Out;

	float env_follower ;
	float curr_ratio = 1;
	float threshold  ;
	float reverse_ratio ;
	float attack ;
	float release ;
	float release_neg ;
	float attack_neg ;

	handle = apdsp->handle;
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

	env_follower = handle->env_follower;

	arm_abs_f32( apCh1In , apCh1Out , data_len);
	arm_abs_f32( apCh2In , apCh2Out , data_len);

	while( data_len-- )
	{
		float tmp;

		tmp = *apCh1Out;
		if(tmp < *apCh2Out)
		{
			tmp =  *apCh2Out;
		}

		if (tmp > env_follower)
		{
			tmp *=attack;
			env_follower *= attack_neg;
		}
		else
		{
			tmp *=release;
			env_follower *= release_neg;
		}
		env_follower += tmp;




		curr_ratio = 1;
		if(env_follower > threshold)
		{
			curr_ratio = threshold/env_follower ;
			tmp = fast_pow(curr_ratio , reverse_ratio);
			curr_ratio = curr_ratio / tmp;
		}

		*apCh1Out++ = (curr_ratio * (*apCh1In++));
		*apCh2Out++ = (curr_ratio * (*apCh2In++));
	}

	handle->env_follower = env_follower;

}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        standard_compressor_ioctl                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t standard_compressor_ioctl(pdsp_descriptor apdsp ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2)
{
	STANDARD_COMPRESSOR_Instance_t *handle;

	handle = apdsp->handle;
	switch(aIoctl_num)
	{
		case IOCTL_DSP_INIT :
			handle->reverse_ratio = 1.0f;//0.5;
			handle->env_follower = 0.0f;
			handle->release = 16.0f;
			handle->attack = 1.0f;
			handle->threshold = 0.99999f;

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


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        standard_compressor_init                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
void  standard_compressor_init(void)
{
	DSP_REGISTER_NEW_MODULE(STANDARD_COMPRESSOR_API_MODULE_NAME ,standard_compressor_ioctl , standard_compressor_dsp , STANDARD_COMPRESSOR_Instance_t);
}

AUTO_INIT_FUNCTION(standard_compressor_init);

