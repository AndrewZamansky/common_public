/*
 *
 * file :   compressor.c
 *
 *
 *
 *
 *
 */



/********  includes *********************/

#include "_compressor_prerequirements_check.h" // should be after {compressor_config.h,dev_management_api.h}

#include "compressor_api.h" //place first to test that header file is self-contained
#include "compressor.h"

#include "math.h"

#include "PRINTF_api.h"

#ifdef CONFIG_USE_HW_DSP
  #include "cpu_config.h"
  #include "arm_math.h"
#endif

/********  defines *********************/
#define THRESHOLD_LIMITER_0db000265			0x0002
#define THRESHOLD_LIMITER_0db000795			0x0004
#define THRESHOLD_LIMITER_0db001855			0x0008
#define THRESHOLD_LIMITER_0db003977			0x0010
#define THRESHOLD_LIMITER_0db008221			0x0020
#define THRESHOLD_LIMITER_0db016716			0x0040
#define THRESHOLD_LIMITER_0db033730			0x0080
#define THRESHOLD_LIMITER_0db067859			0x0100/*0.99221*/
#define THRESHOLD_LIMITER_0db136523			0x0200/*0.98440*/
#define THRESHOLD_LIMITER_0db275500			0x0400/*0.96877*/
#define THRESHOLD_LIMITER_0db560309			0x0800/*0.93752*/
#define THRESHOLD_LIMITER_1db159573			0x1000/*0.87502*/
#define THRESHOLD_LIMITER_2db498509			0x2000/*0.75002*/




/********  types  *********************/

/********  externals *********************/


/********  local defs *********************/

#define INSTANCE(hndl)	((COMPRESSOR_Instance_t*)hndl)


/**********   external variables    **************/



/***********   local variables    **************/


#if 0
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        float_memcpy_with_ratio                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
static void float_memcpy_with_ratio(float *dest ,float *src , size_t len , float ratio)
{
	for( ; len ;len--)
	{
		*dest++ = (*src++) * ratio;
	}
}
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Function:         float_memcpy_with_ratio_2_buffers                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
static void float_memcpy_with_ratio_2_buffers(float *dest1 ,float *src1 ,
		float *dest2 ,float *src2,size_t len,
		float ratio,float step_ratio)
{
	for( ; len ;len--)
	{
		*dest1++ = (*src1++) * ratio;
		*dest2++ = (*src2++) * ratio;
		ratio += step_ratio;
	}
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:         get_max_abs_value_2_buffers                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
static float get_max_abs_value_2_buffers(float *buf1 ,float *buf2 , size_t len , float init_val)
{
	float val ;
 	float max_val = init_val;
	for( ; len ;len--)
	{
		val = (*buf1++) ;
		val = fabsf(val);
		if(val  > max_val)
		{
			max_val = val;
		}
		val = (*buf2++) ;
		val =  fabsf(val);
		if(val  >max_val)
		{
			max_val = val;
		}

	}
	return max_val;
}

#define RELEASE_CHUNK_NUM	16.0f
/*---------------------------------------------------------------------------------------------------------*/
/* Function:         _compressor_buffered_2in_2out                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
void _compressor_buffered_2in_2out(const void * const aHandle ,size_t data_len ,
		dsp_pad_t *in_pads[MAX_NUM_OF_OUTPUT_PADS] , dsp_pad_t out_pads[MAX_NUM_OF_OUTPUT_PADS] )
{
		float *apCh1In ,  *apCh2In;
		float *apCh1Out ,  *apCh2Out;
		//	static int print_count=0;
	//	int threshold_detectd = 0;
		float max_val ;
		float prev_ratio ,tmp_ratio;
		float prev_calculated_ratio ;
		float step_ratio ;
		float reverse_curr_ratio ;
		float curr_ratio = 1;
	//	uint32_t accomulator=0;
		uint8_t usePreviousRatio;
		uint16_t i;//,j;//,k;
		float threshold  ;
		float ratio_change_per_chunk  ;
		float reverse_ratio ;
		float *look_ahead_length_buffer_Ch1 = INSTANCE(aHandle)->look_ahead_length_buffer_Ch1;
		float *look_ahead_length_buffer_Ch2 = INSTANCE(aHandle)->look_ahead_length_buffer_Ch2;
		float release ;
		static float release_ratio_change_per_chunk = 0;
		uint32_t hit_counter;
		uint32_t chunk_size ;
	//	uint32_t look_ahead_length = INSTANCE(aHandle)->look_ahead_length;

		apCh1In = in_pads[0]->buff;
		apCh2In = in_pads[1]->buff;
		apCh1Out = out_pads[0].buff;
		apCh2Out = out_pads[1].buff;

		threshold = INSTANCE(aHandle)->threshold;
		reverse_ratio = INSTANCE(aHandle)->reverse_ratio;
		prev_ratio = INSTANCE(aHandle)->prev_ratio ;
		usePreviousRatio = INSTANCE(aHandle)->usePreviousRatio ;
		chunk_size =  INSTANCE(aHandle)->look_ahead_length ;
		release = INSTANCE(aHandle)->release;
		prev_calculated_ratio = INSTANCE(aHandle)->prev_calculated_ratio;
		hit_counter = INSTANCE(aHandle)->hit_counter;

//		arm_abs_f32( apCh1In , apCh1Out , data_len);
//		arm_abs_f32( apCh2In , apCh2Out , data_len);

		for(i = 0 ; i < data_len ; i+=chunk_size)
		{
			float tmp;//,tmp2;
			//uint32_t dummy_index;

			max_val = get_max_abs_value_2_buffers(&apCh1In[i],&apCh2In[i],chunk_size,threshold);

//			arm_max_f32(&apCh1Out[i],chunk_size,&tmp,&dummy_index);
//			arm_max_f32(&apCh2Out[i],chunk_size,&tmp2,&dummy_index);
//
//			max_val = threshold;
//
//			if(tmp > max_val)
//			{
//				max_val =  tmp;
//			}
//
//			if (tmp2 > max_val)
//			{
//				max_val = tmp2;
//			}


			if(max_val > threshold)
			{
				hit_counter++;
				curr_ratio = threshold/max_val ;
				reverse_curr_ratio = 1.0f/curr_ratio;
				tmp = fast_pow(reverse_curr_ratio , reverse_ratio);
				curr_ratio = curr_ratio * tmp;

				//usePreviousRatio = 1;


			}
			else
			{
				curr_ratio = 1.0f;
			}

			tmp_ratio = curr_ratio;

			if(curr_ratio <= prev_ratio)
			{
				usePreviousRatio = RELEASE_CHUNK_NUM;
				ratio_change_per_chunk = curr_ratio - prev_ratio;

//				prev_calculated_ratio = prev_ratio;
			}
			else
			{

#if 1
				if (RELEASE_CHUNK_NUM == usePreviousRatio)
				{
					//					curr_ratio = (RELEASE_CHUNK_NUM - usePreviousRatio) * curr_ratio ;
					//					curr_ratio += prev_ratio;
					//					curr_ratio /= ((1 + RELEASE_CHUNK_NUM ) - usePreviousRatio);
					//					curr_ratio = prev_ratio;
										ratio_change_per_chunk = 0;
										usePreviousRatio=0;
										release_ratio_change_per_chunk = (1.0f - prev_calculated_ratio)/RELEASE_CHUNK_NUM;

				}
				else
				{
						tmp = (curr_ratio - prev_ratio);
						if(tmp < release_ratio_change_per_chunk)
						{
							release_ratio_change_per_chunk = tmp;
						}
						ratio_change_per_chunk = release_ratio_change_per_chunk;

//						curr_ratio = ((float)(RELEASE_CHUNK_NUM - usePreviousRatio)) * curr_ratio ;
//						curr_ratio += prev_calculated_ratio;
//						curr_ratio /= ((float)((1 + RELEASE_CHUNK_NUM ) - usePreviousRatio));
//						usePreviousRatio--;
//						curr_ratio = prev_calculated_ratio;
//						usePreviousRatio = 1;
				}
#else
				//				if( usePreviousRatio)
				//				{
									ratio_change_per_chunk = (curr_ratio - prev_ratio)/16;
									prev_calculated_ratio = prev_ratio + ratio_change_per_chunk;
				//					if(1 < prev_calculated_ratio)
				//					{
				//						prev_calculated_ratio = 1;
				//						ratio_change_per_chunk = (1 - prev_ratio)/128;
				//					}
				//				}
#endif
			}
			prev_calculated_ratio = tmp_ratio;

//			ratio_change_per_chunk = curr_ratio - prev_ratio;
			step_ratio = ratio_change_per_chunk/chunk_size;


			if(0 == i)
			{
				float_memcpy_with_ratio_2_buffers(&apCh1Out[0], &look_ahead_length_buffer_Ch1[0 ] ,
						&apCh2Out[0], &look_ahead_length_buffer_Ch2[0 ]  , chunk_size,prev_ratio,step_ratio);
			}
			else
			{
				float_memcpy_with_ratio_2_buffers(&apCh1Out[i],
						&apCh1In[i - chunk_size  ] ,
						&apCh2Out[i],
						&apCh2In[i -  chunk_size]  ,chunk_size, prev_ratio,step_ratio);
			}


//			if(curr_ratio < prev_ratio)
//			{
//				chunk_size = COMPRESSOR_CONFIG_CHUNK_SIZE;
//			}
//			else
//			{
//				chunk_size = COMPRESSOR_CONFIG_CHUNK_SIZE/4;
//			}
			prev_ratio += ratio_change_per_chunk;

		}

		float_memcpy_with_ratio_2_buffers(look_ahead_length_buffer_Ch1, &apCh1In[data_len - chunk_size ] ,
				look_ahead_length_buffer_Ch2, &apCh2In[data_len - chunk_size ]  ,chunk_size, 1,0);

		INSTANCE(aHandle)->prev_ratio = prev_ratio;
		INSTANCE(aHandle)->usePreviousRatio = usePreviousRatio ;
		INSTANCE(aHandle)->hit_counter = hit_counter;
		INSTANCE(aHandle)->prev_calculated_ratio = prev_calculated_ratio;

	//	if(print_count > 100)
	//	{
	////		PRINTF_DBG("max_val = %d  \r\n" , max_val);
	//		if(threshold_detectd)
	//		{
	//			PRINTF_DBG("threshold_detectd = %d r=%f \r\n" , threshold_detectd,prev_ratio);
	//		}
	//		threshold_detectd = 0;
	//		print_count = 0;
	//	}
	//	print_count++;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:         _compressor_2in_2out                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
void _compressor_2in_2out(const void * const aHandle , size_t data_len ,
		dsp_pad_t *in_pads[MAX_NUM_OF_OUTPUT_PADS] , dsp_pad_t out_pads[MAX_NUM_OF_OUTPUT_PADS])
{
	float *apCh1In ,  *apCh2In;
	float *apCh1Out ,  *apCh2Out;

	float env_follower_ch1 ;
	float curr_ratio = 1;
	float threshold  ;
	float reverse_ratio ;
	float reverse_curr_ratio ;
	float attack ;
	float release ;
	float release_neg ;
	float attack_neg ;
	float *look_ahead_length_buffer_Ch1 = INSTANCE(aHandle)->look_ahead_length_buffer_Ch1;


	apCh1In = in_pads[0]->buff;
	apCh2In = in_pads[1]->buff;
	apCh1Out = out_pads[0].buff;
	apCh2Out = out_pads[1].buff;

	attack = INSTANCE(aHandle)->attack;
	attack_neg = 1 - attack;
	release = INSTANCE(aHandle)->release;
	release_neg =  1 - release ;
	threshold = INSTANCE(aHandle)->threshold;
	reverse_ratio = INSTANCE(aHandle)->reverse_ratio;

	env_follower_ch1 = *look_ahead_length_buffer_Ch1;

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

		if (tmp > env_follower_ch1)
		{
			tmp *=attack;
			env_follower_ch1 *= attack_neg;
		}
		else
		{
			tmp *=release;
			env_follower_ch1 *= release_neg;
		}
		env_follower_ch1 += tmp;




		curr_ratio = 1;
		if(env_follower_ch1 > threshold)
		{
			curr_ratio = threshold/env_follower_ch1 ;
			reverse_curr_ratio = 1/curr_ratio;
			tmp = fast_pow(reverse_curr_ratio , reverse_ratio);
			curr_ratio = curr_ratio * tmp;
		}

		*apCh1Out++ = (curr_ratio * (*apCh1In++));
		*apCh2Out++ = (curr_ratio * (*apCh2In++));
	}

	*look_ahead_length_buffer_Ch1 = env_follower_ch1 ;


}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        compressor_dsp                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
void compressor_dsp(const void * const aHandle , size_t data_len ,
		dsp_pad_t *in_pads[MAX_NUM_OF_OUTPUT_PADS] , dsp_pad_t out_pads[MAX_NUM_OF_OUTPUT_PADS])
{


	switch(INSTANCE(aHandle)->type)
	{
		case COMPRESSOR_API_TYPE_LOOKAHEAD:
				_compressor_buffered_2in_2out(aHandle , data_len ,
						in_pads , out_pads);
			break ;
		case COMPRESSOR_API_TYPE_REGULAR:
				_compressor_2in_2out(aHandle , data_len ,
						in_pads , out_pads);
			break ;
	}



}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        compressor_set_buffers                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
void compressor_set_buffers(COMPRESSOR_Instance_t *pInstance,uint32_t buffer_size)
{
	pInstance->look_ahead_length_buffer_Ch1 =
			(float*)realloc(pInstance->look_ahead_length_buffer_Ch1, buffer_size*sizeof(float));
	pInstance->look_ahead_length_buffer_Ch2 =
			(float*)realloc(pInstance->look_ahead_length_buffer_Ch2, buffer_size*sizeof(float));

}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        compressor_ioctl                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t compressor_ioctl(void * const aHandle ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2)
{
	uint32_t look_ahead_length;
	switch(aIoctl_num)
	{
//
//		case IOCTL_GET_PARAMS_ARRAY_FUNC :
//			*(const dev_param_t**)aIoctl_param1  = COMPRESSOR_Dev_Params;
//			*(uint8_t*)aIoctl_param2 = sizeof(COMPRESSOR_Dev_Params)/sizeof(dev_param_t); //size
//			break;
//

		case IOCTL_DEVICE_START :
			switch(INSTANCE(aHandle)->type)
			{
				case COMPRESSOR_API_TYPE_LOOKAHEAD:
					look_ahead_length = INSTANCE(aHandle)->look_ahead_length ;
					compressor_set_buffers(INSTANCE(aHandle) , look_ahead_length);
					INSTANCE(aHandle)->release = 64;
					break ;
				case COMPRESSOR_API_TYPE_REGULAR:
					compressor_set_buffers(INSTANCE(aHandle) , 1);
					break ;
			}

			break;
		case IOCTL_COMPRESSOR_SET_HIGH_THRESHOLD :
			INSTANCE(aHandle)->threshold = *((float*)aIoctl_param1);
			break;
		case IOCTL_COMPRESSOR_SET_LOOK_AHEAD_SIZE :
			look_ahead_length = (uint32_t)((size_t)aIoctl_param1);
			INSTANCE(aHandle)->look_ahead_length = look_ahead_length;
			compressor_set_buffers(INSTANCE(aHandle) , look_ahead_length);
			break;
		case IOCTL_COMPRESSOR_SET_RATIO :
			INSTANCE(aHandle)->reverse_ratio = 1/(*((float*)aIoctl_param1));
			break;
		case IOCTL_COMPRESSOR_SET_TYPE :
			INSTANCE(aHandle)->type = (uint8_t)((size_t)aIoctl_param1);
			break;
		case IOCTL_COMPRESSOR_SET_ATTACK :
			INSTANCE(aHandle)->attack = *((float*)aIoctl_param1);
			break;
		case IOCTL_COMPRESSOR_SET_RELEASE :
			INSTANCE(aHandle)->release = *((float*)aIoctl_param1);
			break;
		case IOCTL_COMPRESSOR_GET_HIT_COUNTER :
			*((uint32_t*)aIoctl_param1) = INSTANCE(aHandle)->hit_counter;
			INSTANCE(aHandle)->hit_counter = 0;
			break;
		default :
			return 1;
	}
	return 0;
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        COMPRESSOR_API_Init_Dev_Descriptor                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t  compressor_api_init_dsp_descriptor(pdsp_descriptor aDspDescriptor)
{
	COMPRESSOR_Instance_t *pInstance;

	if(NULL == aDspDescriptor) return 1;

	pInstance = (COMPRESSOR_Instance_t *)malloc(sizeof(COMPRESSOR_Instance_t));
	if(NULL == pInstance) return 1;

	aDspDescriptor->handle = pInstance;
	aDspDescriptor->ioctl = compressor_ioctl;
	aDspDescriptor->dsp_func = compressor_dsp;
	pInstance->reverse_ratio = 0;//0.5;
	pInstance->prev_ratio = 1;
	pInstance->usePreviousRatio = 1;
	pInstance->look_ahead_length_buffer_Ch1 = NULL;
	pInstance->look_ahead_length_buffer_Ch2 = NULL;
	pInstance->look_ahead_length = CONFIG_COMPRESSOR_DEFAULT_CHUNK_SIZE;
	pInstance->release = 64;

	return 0 ;

}


