/*
 *
 *   file  :  ADC.cpp
 *
 */



/***************   includes    *******************/
#include "_adc_stm8_prerequirements_check.h" // should be after {uart_stm8_config.h,dev_management_api.h}

#include "dev_management_api.h" // for device manager defines and typedefs
#include "adc_api.h"

#include "adc_stm8_api.h"
#include "adc_stm8.h"

#include "stm8s.h"
#include "stm8s_adc1.h"

/***************   defines    *******************/

#define ADC_STM8_CONFIG_VCC_mV 			3300

/***************   typedefs    *******************/

/**********   external variables    **************/

void ADC1_Init_Reduced(ADC1_PresSel_TypeDef ADC1_PrescalerSelection, ADC1_ExtTrig_TypeDef ADC1_ExtTrigger,
		FunctionalState ADC1_ExtTriggerState, ADC1_SchmittTrigg_TypeDef ADC1_SchmittTriggerChannel,
		FunctionalState ADC1_SchmittTriggerState);


/***********   local variables    **************/


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
uint8_t adc_stm8_ioctl( struct dev_desc_t *adev ,const uint8_t aIoctl_num
		, void * aIoctl_param1 , void * aIoctl_param2)
{
	adc_stm8_instance_t *config_handle;

	config_handle = DEV_GET_CONFIG_DATA_POINTER(adev);
	switch(aIoctl_num)
	{
		case IOCTL_ADC_GET_CURRENT_VALUE_mV :
			ADC1_ConversionConfig(
					ADC1_CONVERSIONMODE_SINGLE,config_handle->channel,
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
