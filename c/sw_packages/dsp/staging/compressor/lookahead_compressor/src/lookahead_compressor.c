/*
 *
 * file :   lookahead_compressor.c
 *
 *
 *
 *
 *
 */



/********  includes *********************/

#include "_lookahead_compressor_prerequirements_check.h" // should be after {lookahead_compressor_config.h,dev_management_api.h}

#include "lookahead_compressor_api.h" //place first to test that header file is self-contained
#include "lookahead_compressor.h"

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

char lookahead_compressor_module_name[] = "lookahead_compressor";


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




/*---------------------------------------------------------------------------------------------------------*/
/* Function:        lookahead_compressor_dsp                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
void lookahead_compressor_dsp(pdsp_descriptor apdsp , size_t data_len ,
		dsp_pad_t *in_pads[MAX_NUM_OF_OUTPUT_PADS] , dsp_pad_t out_pads[MAX_NUM_OF_OUTPUT_PADS])
{
	LOOKAHEAD_COMPRESSOR_Instance_t *handle ;
	float *apCh1In ,  *apCh2In;
	float *apCh1Out ,  *apCh2Out;
	float max_val ;
	float prev_ratio ,tmp_ratio;
	float prev_calculated_ratio ;
	float step_ratio ;
	float curr_ratio = 1;
	float usePreviousRatio;
	uint16_t i ;
	float threshold  ;
	float ratio_change_per_chunk  ;
	float reverse_ratio ;
	float *look_ahead_length_buffer_Ch1;
	float *look_ahead_length_buffer_Ch2;
	float release ;
	float attack ;
	float release_ratio_change_per_chunk ;
	uint32_t hit_counter;
	uint32_t chunk_size ;

	handle=apdsp->handle;
	look_ahead_length_buffer_Ch1 = handle->look_ahead_length_buffer_Ch1;
	look_ahead_length_buffer_Ch2 = handle->look_ahead_length_buffer_Ch2;

	apCh1In = in_pads[0]->buff;
	apCh2In = in_pads[1]->buff;
	apCh1Out = out_pads[0].buff;
	apCh2Out = out_pads[1].buff;

	if (LOOKAHEAD_COMPRESSOR_API_TYPE_LIMITER == handle->type )
	{
		reverse_ratio = 0;
		attack = 1;
	}
	else
	{
		reverse_ratio = handle->reverse_ratio;
		attack = handle->attack;
	}

	threshold = handle->threshold;
	prev_ratio = handle->prev_ratio ;
	usePreviousRatio = handle->usePreviousRatio ;
	chunk_size =  handle->look_ahead_length ;
	release = handle->release;
	prev_calculated_ratio = handle->prev_calculated_ratio;
	hit_counter = handle->hit_counter;
	release_ratio_change_per_chunk = handle->release_ratio_change_per_chunk ;

	if(0 == chunk_size)
		return;

	for(i = 0 ; i < data_len ; i+=chunk_size)
	{
		float tmp;

		max_val = get_max_abs_value_2_buffers(&apCh1In[i],&apCh2In[i],chunk_size,threshold);


		if(max_val > threshold)
		{
			hit_counter++;
			curr_ratio = threshold/max_val ;
			tmp = fast_pow(curr_ratio , reverse_ratio);
			curr_ratio = curr_ratio / tmp;
		}
		else
		{
			curr_ratio = 1.0f;
		}

		tmp_ratio = curr_ratio;

		if(curr_ratio <= prev_ratio)
		{
			usePreviousRatio = release;
			ratio_change_per_chunk = (curr_ratio - prev_ratio)/attack;
		}
		else
		{
			if (release == usePreviousRatio)
			{
				ratio_change_per_chunk = 0;
				usePreviousRatio=0;
				release_ratio_change_per_chunk = (1.0f - prev_calculated_ratio)/release;

			}
			else
			{
				float target_ratio;
				target_ratio = prev_ratio + release_ratio_change_per_chunk;
				if(prev_calculated_ratio < target_ratio)
				{
					target_ratio = prev_calculated_ratio;
				}
				if(curr_ratio < target_ratio)
				{
					target_ratio = curr_ratio;
				}

				ratio_change_per_chunk = target_ratio - prev_ratio;

			}
		}
		
		// TODO : test if following needed
#if 0		
		if ((ratio_change_per_chunk < 0.000001) &&
			(ratio_change_per_chunk > -0.000001))
		{
			ratio_change_per_chunk = 0;
		}
#endif		
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
		prev_calculated_ratio = tmp_ratio;


		prev_ratio += ratio_change_per_chunk;

	}

	float_memcpy_with_ratio_2_buffers(look_ahead_length_buffer_Ch1, &apCh1In[data_len - chunk_size ] ,
			look_ahead_length_buffer_Ch2, &apCh2In[data_len - chunk_size ]  ,chunk_size, 1,0);

	handle->prev_ratio = prev_ratio;
	handle->usePreviousRatio = usePreviousRatio ;
	handle->hit_counter = hit_counter;
	handle->prev_calculated_ratio = prev_calculated_ratio;
	handle->release_ratio_change_per_chunk = release_ratio_change_per_chunk;

	//{
	//	static int print_count=0;
	//	int threshold_detectd = 0;
	//
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
	//}
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        lookahead_compressor_set_buffers                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
void lookahead_compressor_set_buffers(LOOKAHEAD_COMPRESSOR_Instance_t *pInstance,uint32_t buffer_size)
{
	pInstance->look_ahead_length_buffer_Ch1 =
			(float*)realloc(pInstance->look_ahead_length_buffer_Ch1, buffer_size*sizeof(float));
	pInstance->look_ahead_length_buffer_Ch2 =
			(float*)realloc(pInstance->look_ahead_length_buffer_Ch2, buffer_size*sizeof(float));

}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        lookahead_compressor_ioctl                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t lookahead_compressor_ioctl(pdsp_descriptor apdsp ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2)
{
	LOOKAHEAD_COMPRESSOR_Instance_t *handle;
	uint32_t look_ahead_length;

	handle = apdsp->handle;
	switch(aIoctl_num)
	{
		case IOCTL_DSP_INIT :
			handle->reverse_ratio = 0.0f;//0.5;
			handle->prev_ratio = 0.0f;
			handle->prev_calculated_ratio = 0.5f;
			handle->usePreviousRatio = 16.0f;
			handle->look_ahead_length_buffer_Ch1 = NULL;
			handle->look_ahead_length_buffer_Ch2 = NULL;
			handle->release_ratio_change_per_chunk = 0.0f ;
			handle->look_ahead_length = 0.0f;
			handle->release = 16.0f;
			handle->attack = 1.0f;
			handle->threshold = 0.99999f;


			break;
		case IOCTL_LOOKAHEAD_COMPRESSOR_SET_HIGH_THRESHOLD :
			handle->threshold = *((float*)aIoctl_param1);
			break;
		case IOCTL_LOOKAHEAD_COMPRESSOR_SET_LOOK_AHEAD_SIZE :
			look_ahead_length = (uint32_t)((size_t)aIoctl_param1);
			handle->look_ahead_length = look_ahead_length;
			lookahead_compressor_set_buffers(handle , look_ahead_length);
			break;
		case IOCTL_LOOKAHEAD_COMPRESSOR_SET_RATIO :
			handle->reverse_ratio = 1/(*((float*)aIoctl_param1));
			break;
		case IOCTL_LOOKAHEAD_COMPRESSOR_SET_TYPE :
			handle->type = (uint8_t)((size_t)aIoctl_param1);
			break;
		case IOCTL_LOOKAHEAD_COMPRESSOR_SET_ATTACK :
			handle->attack = *((float*)aIoctl_param1);
			break;
		case IOCTL_LOOKAHEAD_COMPRESSOR_SET_RELEASE :
			handle->release = *((float*)aIoctl_param1);
			break;
		case IOCTL_LOOKAHEAD_COMPRESSOR_GET_HIT_COUNTER :
			*((uint32_t*)aIoctl_param1) = handle->hit_counter;
			handle->hit_counter = 0;
			break;
		default :
			return 1;
	}
	return 0;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        lookahead_compressor_init                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
void  lookahead_compressor_init(void)
{
	DSP_REGISTER_NEW_MODULE(LOOKAHEAD_COMPRESSOR_API_MODULE_NAME ,lookahead_compressor_ioctl , lookahead_compressor_dsp , LOOKAHEAD_COMPRESSOR_Instance_t);
}

AUTO_INIT_FUNCTION(lookahead_compressor_init);

