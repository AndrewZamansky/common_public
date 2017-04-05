/*
 *
 *   file  :  ADC.cpp
 *
 */



/***************   includes    *******************/
#include "_project_typedefs.h"
#include "_project_defines.h"
#include "_project_func_declarations.h"

#include "dev_management_api.h" // for device manager defines and typedefs

#include "adc_api.h"
#include "adc_stm32f10x_api.h"
#include "adc_stm32f10x.h"

#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_adc.h"


/***************   defines    *******************/

#define P_INSTANCE(pHndl)  ((ADC_Instance_t*)pHndl)
#define ADC_HAL_MAX_NUM_OF_ADCS 	2

/***************   typedefs    *******************/

/**********   external variables    **************/



/***********   local variables    **************/
static ADC_Instance_t ADC_InstanceParams[ADC_HAL_MAX_NUM_OF_ADCS] = { {0} };
static uint16_t usedInstances =0 ;

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
static uint8_t adc_ioctl( struct dev_desc_t *adev ,const uint8_t aIoctl_num
		, void * aIoctl_param1 , void * aIoctl_param2)
{
	ADC_InitTypeDef lADC_InitStruct;
	GPIO_InitTypeDef GPIO_InitStructure;

	switch(aIoctl_num)
	{
		case IOCTL_GET_PARAMS_ARRAY_FUNC :
//			*aIoctl_param1  = ADC_Dev_Params;
//			*aIoctl_param2 = (void*)sizeof(ADC_Dev_Params)/sizeof(dev_param_t); //size
			*(uint8_t*)aIoctl_param2 = 0; // size
			break;
		case IOCTL_ADC_GET_CURRENT_VALUE_mV :

			ADC_Cmd(ADC1, ENABLE);
			while (RESET == ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC))
			{
				vTaskDelay(10);
			}

			ADC_ClearFlag(ADC1,ADC_FLAG_EOC);

			*(uint32_t*)aIoctl_param1 = ( (3.3 * ADC_GetConversionValue(ADC1) *100) / 4096 );
			break;
		case IOCTL_DEVICE_START :

			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 ;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

			GPIO_Init( GPIOA, &GPIO_InitStructure );



			RCC_APB2PeriphClockCmd(	RCC_APB2Periph_ADC1 , ENABLE );
			RCC_ADCCLKConfig(RCC_PCLK2_Div6);

			ADC_DeInit(ADC1);

			ADC_StructInit(&lADC_InitStruct);
			lADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
			ADC_Init(ADC1, &lADC_InitStruct);
			ADC_RegularChannelConfig(ADC1,ADC_Channel_0,1,ADC_SampleTime_239Cycles5);

			ADC_Cmd(ADC1, ENABLE); // first write to ADC1 will power-up adc

			ADC_ResetCalibration(ADC1);
			while (SET == ADC_GetResetCalibrationStatus(ADC1)) {}
			ADC_StartCalibration(ADC1);
			while (SET == ADC_GetCalibrationStatus(ADC1)) {}


			break;

		default :
			return 1;
	}
	return 0;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        adc_stm32f10x_api_init_dev_descriptor                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t  adc_stm32f10x_api_init_dev_descriptor(struct dev_desc_t *aDevDescriptor)
{
	if(NULL == aDevDescriptor) return 1;
	if (usedInstances >= ADC_HAL_MAX_NUM_OF_ADCS) return 1;


	aDevDescriptor->handle = &ADC_InstanceParams[usedInstances ];
	aDevDescriptor->ioctl = adc_ioctl;
	usedInstances++;

	return 0 ;

}

