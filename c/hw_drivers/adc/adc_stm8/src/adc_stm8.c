/*
 *
 *   file  :  ADC.cpp
 *
 */



/***************   includes    *******************/
#include "adc_stm8_config.h"
#include "dev_managment_api.h" // for device manager defines and typedefs
#include "adc_api.h"
#include "src/_adc_stm8_prerequirements_check.h" // should be after {uart_stm8_config.h,dev_managment_api.h}

#include "adc_stm8_api.h"

#include "stm8s.h"
#include "stm8s_adc1.h"

/***************   defines    *******************/

#define P_INSTANCE(pHndl)  ((ADC_STM8_Instance_t*)pHndl)
#define ADC_HAL_MAX_NUM_OF_ADCS 	2

/***************   typedefs    *******************/

/**********   external variables    **************/

void ADC1_Init_Reduced(ADC1_PresSel_TypeDef ADC1_PrescalerSelection, ADC1_ExtTrig_TypeDef ADC1_ExtTrigger,
		FunctionalState ADC1_ExtTriggerState, ADC1_SchmittTrigg_TypeDef ADC1_SchmittTriggerChannel,
		FunctionalState ADC1_SchmittTriggerState);


/***********   local variables    **************/
#if ADC_STM8_CONFIG_NUM_OF_DYNAMIC_INSTANCES > 0
	static ADC_STM8_Instance_t ADC_InstanceParams[ADC_HAL_MAX_NUM_OF_ADCS] = { {0} };
	static uint16_t usedInstances =0 ;
#endif

//static const dev_param_t ADC_Dev_Params[]=
//{
//		{IOCTL_ADC_SET_CHIP_TYPE , IOCTL_VOID , (uint8_t*)ADC_API_CHIP_TYPE_STR, NOT_FOR_SAVE},
//};



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        adc_ioctl                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t adc_stm8_ioctl( void * const aHandle ,const uint8_t aIoctl_num
		, void * aIoctl_param1 , void * aIoctl_param2)
{

	switch(aIoctl_num)
	{
		case IOCTL_ADC_GET_CURRENT_VALUE_mV :
			ADC1_ConversionConfig(
					ADC1_CONVERSIONMODE_SINGLE,P_INSTANCE(aHandle)->channel,
					ADC1_ALIGN_RIGHT
					);
			os_delay_ms(1);
			ADC1_StartConversion();
			while (RESET == ADC1_GetFlagStatus(ADC1_FLAG_EOC))
			{
				os_delay_ms(5);
			}

			ADC1_ClearFlag(ADC1_FLAG_EOC);

			*(uint32_t*)aIoctl_param1 =
					 ( (uint32_t)ADC1_GetConversionValue()  * ADC_STM8_CONFIG_VCC_mV ) >>10 ; // (3.3V * x / 1024)
			break;
		case IOCTL_DEVICE_START :
			ADC1_DeInit();

			ADC1_Init_Reduced(ADC1_PRESSEL_FCPU_D2,
									ADC1_EXTTRIG_TIM,
									DISABLE,
									ADC1_SCHMITTTRIG_ALL,
									DISABLE);


		default :
			return 1;
	}
	return 0;
}

#if ADC_STM8_CONFIG_NUM_OF_DYNAMIC_INSTANCES > 0

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        adc_stm8_api_init_dev_descriptor                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t  adc_stm8_api_init_dev_descriptor(pdev_descriptor aDevDescriptor)
{
	if(NULL == aDevDescriptor) return 1;
	if (usedInstances >= ADC_HAL_MAX_NUM_OF_ADCS) return 1;


	aDevDescriptor->handle = &ADC_InstanceParams[usedInstances ];
	aDevDescriptor->ioctl = adc_stm8_ioctl;
	usedInstances++;

	return 0 ;

}
#endif
