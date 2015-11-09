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

#include "compressor_config.h"
#include "dev_managment_api.h" // for device manager defines and typedefs
#include "dsp_managment_api.h" // for device manager defines and typedefs
#include "_compressor_prerequirements_check.h" // should be after {compressor_config.h,dev_managment_api.h}

#include "compressor_api.h" //place first to test that header file is self-contained
#include "compressor.h"
#include "common_dsp_api.h"

#include "math.h"

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
#if COMPRESSOR_CONFIG_NUM_OF_DYNAMIC_INSTANCES>0

	static COMPRESSOR_Instance_t COMPRESSOR_InstanceParams[COMPRESSOR_CONFIG_NUM_OF_DYNAMIC_INSTANCES] = { {0} };
	static uint16_t usedInstances =0 ;


#endif // for COMPRESSOR_CONFIG_NUM_OF_DYNAMIC_INSTANCES>0


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
void compressor_dsp(const void * const aHandle ,
		uint8_t num_of_inputs,uint8_t num_of_ouputs, size_t data_len ,
		float *apCh1In , float *apCh2In,
		float *apCh1Out , float *apCh2Out)
{

	static int print_count=0;
	float max_val ;
	static float prev_ratio = 1;
	float curr_ratio = 1;
	static int threshold_detectd = 0;
//	uint32_t accomulator=0;
	uint8_t usePreviousRatio;
	uint16_t i,j,k;
	float threshold = INSTANCE(aHandle)->threshold;
	float reverse_ratio = INSTANCE(aHandle)->reverse_ratio;

	max_val = threshold ;
	j = 0;
	usePreviousRatio = 1;
	for(i = 0 ; i < data_len ; i++)
	{
		uint32_t tmp;
		tmp = abs(apCh1In[i]);

		//accomulator += ( tmp + THRESHOLD_LIMITER_0db560309)  & 0xffff0000;
		if (tmp > max_val)
		{
			max_val = tmp;
//			accomulator =1 ;
		}

		tmp = abs(apCh2In[i]);
		if (tmp > max_val)
		{
			max_val = tmp;
//			accomulator =1 ;
		}

		if(( (COMPRESSOR_CONFIG_CHUNK_SIZE-1) == j) /*|| ((buff_len-1) == i)*/)
		{
			if(max_val > threshold)
			{
				curr_ratio = threshold/max_val ;
				curr_ratio = curr_ratio * powf(1/curr_ratio,reverse_ratio);

				threshold_detectd++;
				usePreviousRatio = 1;
				if(COMPRESSOR_CONFIG_CHUNK_SIZE < i)
				{
					//if(curr_ratio < prev_ratio) should be if ((2 / (1 + (1/curr_ratio))  < prev_ratio)
					if(1 == prev_ratio)
					{
						prev_ratio = 2 / (1 + (1/curr_ratio) ); //  = thr/((max_val-thr)/2+thr)
					}
				}
			}
			else
			{
				if(usePreviousRatio)
				{
					curr_ratio = 2 / (1 + (1/prev_ratio) ); //  = thr/((max_val-thr)/2+thr)
					usePreviousRatio=0;
				}
				else
				{
					curr_ratio = 1;
				}

			}

			if(COMPRESSOR_CONFIG_CHUNK_SIZE < i)
			{
				for( k = (i - (2*COMPRESSOR_CONFIG_CHUNK_SIZE - 1)) ; k <= (i - COMPRESSOR_CONFIG_CHUNK_SIZE) ; k++)
				{


					apCh1Out[ k ] =  prev_ratio * apCh1In[k]   ;
					apCh2Out[ k ] =  prev_ratio * apCh2In[k] 	;
				}
			}

			prev_ratio = curr_ratio;

			max_val = threshold;
			j=0;
		}
		else
		{
			j++;
		}
	}


	if(print_count > 100)
	{
//		PRINTF_DBG("max_val = %d  \r\n" , max_val);
		if(threshold_detectd)
		{
			PRINTF_DBG("threshold_detectd = %d  \r\n" , threshold_detectd);
		}
		threshold_detectd = 0;
		print_count = 0;
	}
	print_count++;


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

	switch(aIoctl_num)
	{
//#if COMPRESSOR_CONFIG_NUM_OF_DYNAMIC_INSTANCES > 0
//		case IOCTL_GET_PARAMS_ARRAY_FUNC :
//			*(const dev_param_t**)aIoctl_param1  = COMPRESSOR_Dev_Params;
//			*(uint8_t*)aIoctl_param2 = sizeof(COMPRESSOR_Dev_Params)/sizeof(dev_param_t); //size
//			break;
//#endif // for COMPRESSOR_CONFIG_NUM_OF_DYNAMIC_INSTANCES > 0


		case IOCTL_DEVICE_START :

			break;
		case IOCTL_COMPRESSOR_SET_HIGH_THRESHOLD :
			INSTANCE(aHandle)->threshold = (float)((size_t)aIoctl_param1);
			break;
		case IOCTL_COMPRESSOR_SET_RATIO :
			INSTANCE(aHandle)->reverse_ratio = 1/(float)((size_t)aIoctl_param1);
			break;
		default :
			return 1;
	}
	return 0;
}





#if COMPRESSOR_CONFIG_NUM_OF_DYNAMIC_INSTANCES>0

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
	if (usedInstances >= COMPRESSOR_CONFIG_NUM_OF_DYNAMIC_INSTANCES) return 1;

	pInstance = &COMPRESSOR_InstanceParams[usedInstances ];

	aDspDescriptor->handle = pInstance;
	aDspDescriptor->ioctl = compressor_ioctl;
	aDspDescriptor->dsp_func = compressor_dsp;
	pInstance->reverse_ratio = 0;//0.5;
	usedInstances++;

	return 0 ;

}
#endif  // for COMPRESSOR_CONFIG_NUM_OF_DYNAMIC_INSTANCES>0


