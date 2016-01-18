/*
 *
 * file :   I2S_splitter.c
 *
 *
 *
 *
 *
 */



/********  includes *********************/

#include "I2S_splitter_config.h"
#include "dev_managment_api.h" // for device manager defines and typedefs
#include "dsp_managment_api.h" // for device manager defines and typedefs
#include "_I2S_splitter_prerequirements_check.h" // should be after {I2S_splitter_config.h,dev_managment_api.h}

#include "I2S_splitter_api.h" //place first to test that header file is self-contained
#include "I2S_splitter.h"
#include "common_dsp_api.h"


/********  defines *********************/

#if (2==I2S_SPLITTER_CONFIG_NUM_OF_BYTES_PER_AUDIO_WORD)
	#define	FLOAT_NORMALIZER	0x7fff
	typedef int16_t	buffer_type_t	;
#endif
#if (4==I2S_SPLITTER_CONFIG_NUM_OF_BYTES_PER_AUDIO_WORD)
	#define	FLOAT_NORMALIZER	0x7fffffff
	typedef int32_t	buffer_type_t	;
#endif

/********  types  *********************/

/********  externals *********************/


/********  local defs *********************/

#define INSTANCE(hndl)	((I2S_SPLITTER_Instance_t*)hndl)


/**********   external variables    **************/



/***********   local variables    **************/
#if I2S_SPLITTER_CONFIG_NUM_OF_DYNAMIC_INSTANCES>0

	static I2S_SPLITTER_Instance_t I2S_SPLITTER_InstanceParams[I2S_SPLITTER_CONFIG_NUM_OF_DYNAMIC_INSTANCES] = { {0} };
	static uint16_t usedInstances =0 ;


#endif // for I2S_SPLITTER_CONFIG_NUM_OF_DYNAMIC_INSTANCES>0



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        I2S_splitter_dsp                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
void I2S_splitter_dsp(const void * const aHandle , size_t data_len ,
		dsp_pad_t *in_pads[MAX_NUM_OF_OUTPUT_PADS] , dsp_pad_t out_pads[MAX_NUM_OF_OUTPUT_PADS])
{
	float *apCh1Out ,  *apCh2Out;


	apCh1Out = out_pads[0].buff;
	apCh2Out = out_pads[1].buff;

	buffer_type_t *pRxBuf;
	pRxBuf = (buffer_type_t *)in_pads[0]->buff;
	for( ; data_len ;data_len--)
	{
		*apCh1Out++ = ((float) (*pRxBuf++)) / FLOAT_NORMALIZER;//  pRxBuf[2*j ];
		*apCh2Out++ = ((float) (*pRxBuf++)) / FLOAT_NORMALIZER;//  pRxBuf[2*j + 1];
	}

}





/*---------------------------------------------------------------------------------------------------------*/
/* Function:        I2S_splitter_ioctl                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t I2S_splitter_ioctl(void * const aHandle ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2)
{
	switch(aIoctl_num)
	{
//#if I2S_SPLITTER_CONFIG_NUM_OF_DYNAMIC_INSTANCES > 0
//		case IOCTL_GET_PARAMS_ARRAY_FUNC :
//			*(const dev_param_t**)aIoctl_param1  = I2S_SPLITTER_Dev_Params;
//			*(uint8_t*)aIoctl_param2 = sizeof(I2S_SPLITTER_Dev_Params)/sizeof(dev_param_t); //size
//			break;
//#endif // for I2S_SPLITTER_CONFIG_NUM_OF_DYNAMIC_INSTANCES > 0


		case IOCTL_DEVICE_START :


			break;

		default :
			return 1;
	}
	return 0;
}





#if I2S_SPLITTER_CONFIG_NUM_OF_DYNAMIC_INSTANCES>0

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        I2S_SPLITTER_API_Init_Dev_Descriptor                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t  I2S_splitter_api_init_dsp_descriptor(pdsp_descriptor aDspDescriptor)
{
	I2S_SPLITTER_Instance_t *pInstance;
	if(NULL == aDspDescriptor) return 1;
	if (usedInstances >= I2S_SPLITTER_CONFIG_NUM_OF_DYNAMIC_INSTANCES) return 1;

	pInstance = &I2S_SPLITTER_InstanceParams[usedInstances ];

	aDspDescriptor->handle = pInstance;
	aDspDescriptor->ioctl = I2S_splitter_ioctl;
	aDspDescriptor->dsp_func = I2S_splitter_dsp;


	usedInstances++;

	return 0 ;

}
#endif  // for I2S_SPLITTER_CONFIG_NUM_OF_DYNAMIC_INSTANCES>0


