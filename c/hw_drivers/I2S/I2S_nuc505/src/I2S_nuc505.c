/*
 *
 * I2S_stm32f10x.c
 *
 *
 *
 *
 *
 */

/********  includes *********************/

#include "src/_I2S_nuc505_prerequirements_check.h" // should be after {I2S_nuc505_config.h,dev_management_api.h}

#include "I2S_nuc505_api.h"
#include "I2S_nuc505.h"
#include "irq_api.h"
#include "timer_wrapper_api.h"


#include "NUC505Series.h"

#include "I2S_nuc505_add_component.h"

/********  defines *********************/


/********  types  *********************/


/* ---------------------------- External variables ---------------------------------*/

/* ------------------------ External functions ------------------------------*/

/* ------------------------ Exported variables ------------------------------*/


/********  local defs *********************/

static I2S_nuc505_instance_t *pI2SHandle;


uint16_t num_of_words_in_buffer_per_chenel = 0;
uint8_t	num_of_bytes_in_word;

uint8_t *PcmRxBuff;//[2][I2S_BUFF_LEN*2] = {{0}};
uint8_t *PcmTxBuff;//[2][I2S_BUFF_LEN*2] = {{0}};
uint8_t *PcmRxThrBuff;
uint8_t *PcmTxThrBuff;

uint8_t start_flag;

uint8_t i2s_loopback = 0;
float i2s_nuc505_dbg_gain = 1.0f;

#define ACCUMULATION_TIMES	10
uint32_t relax=3;
volatile uint32_t accumultor_timer = 0;
volatile uint32_t saved_accumultor_timer = 0;
volatile uint32_t accumultor_counter = 0;
uint8_t   	clock_mode;

void __attribute__((section(".critical_text"))) I2S_IRQHandler(void)
{
	uint32_t u32I2SIntFlag;
	uint8_t *pRxBuf=NULL;
	uint8_t *pTxBuf=NULL;
	uint16_t i;

#if	CONFIG_I2S_NUC505_USE_SAMPLE_RATE_DETECTION
	
	if (I2S_NUC505_API_MASTER_MODE != clock_mode)
	{
		uint64_t	timer_counter;

		DEV_IOCTL(pI2SHandle->timer_dev, IOCTL_TIMER_WRAPPER_API_GET_COUNTER, (void*)&timer_counter);
		DEV_IOCTL_0_PARAMS(pI2SHandle->timer_dev, IOCTL_TIMER_WRAPPER_API_RESTART_COUNTER);

		if(relax == 0)
		{
			if(ACCUMULATION_TIMES == accumultor_counter )
			{
				saved_accumultor_timer = accumultor_timer;
				accumultor_timer = 0;
				accumultor_counter = 0;
			}
			else
			{
				accumultor_timer += timer_counter;
				accumultor_counter++;
			}
		}
		else
		{
			relax--;
		}
	}
#endif

	u32I2SIntFlag = I2S_GET_INT_FLAG(I2S, (I2S_STATUS_RDMATIF_Msk | I2S_STATUS_RDMAEIF_Msk));

	/* Copy RX data to TX buffer */
	if (u32I2SIntFlag & I2S_STATUS_RDMATIF_Msk)
	{
		pRxBuf = PcmRxBuff;
		pTxBuf = PcmTxBuff;
		I2S_CLR_INT_FLAG(I2S, I2S_STATUS_RDMATIF_Msk);
	}
	else if (u32I2SIntFlag & I2S_STATUS_RDMAEIF_Msk)
	{
		pRxBuf = PcmRxThrBuff;
		pTxBuf = PcmTxThrBuff;
		I2S_CLR_INT_FLAG(I2S, I2S_STATUS_RDMAEIF_Msk);

		if ( start_flag == 1 )
		{
			start_flag = 0;
			I2S_ENABLE_TXDMA(I2S);
			I2S_ENABLE_TX(I2S);
		}
	}

	if(NULL != pRxBuf)
	{
		if(0 == i2s_loopback)
		{
			if(NULL !=pI2SHandle->callback_dev)
			{
				DEV_CALLBACK_2_PARAMS( pI2SHandle->callback_dev ,
						CALLBACK_NEW_DATA_ARRIVED, pRxBuf ,  pTxBuf) ;
			}
		}
		else
		{
			if(2 == num_of_bytes_in_word)
			{
				int16_t *pCurrRxBuf = (int16_t *)pRxBuf;
				int16_t *pCurrTxBuf = (int16_t *)pTxBuf;
				for(i = 0 ; i < num_of_words_in_buffer_per_chenel ; i++)
				{
					*pCurrTxBuf++ =  (int16_t)(i2s_nuc505_dbg_gain *  (float)(*pCurrRxBuf++));
					*pCurrTxBuf++ =  (int16_t)(i2s_nuc505_dbg_gain *  (float)(*pCurrRxBuf++));
				}
			}
			else if(4 == num_of_bytes_in_word)
			{
				int32_t *pCurrRxBuf = (int32_t *)pRxBuf;
				int32_t *pCurrTxBuf = (int32_t *)pTxBuf;
				for(i = 0 ; i < num_of_words_in_buffer_per_chenel ; i++)
				{
					*pCurrTxBuf++ =  (int32_t)(i2s_nuc505_dbg_gain *  (float)(*pCurrRxBuf++));
					*pCurrTxBuf++ =  (int32_t)(i2s_nuc505_dbg_gain *  (float)(*pCurrRxBuf++));
				}
			}
		}
	}
}




 void demo_LineIn(void)
 {
 	uint32_t i;

 		// Setting Right Line In Channel
 		SYS->GPD_MFPL  = (SYS->GPD_MFPL & (~SYS_GPD_MFPL_PD4MFP_Msk) ) | SYS_GPD_MFPL_PD4MFP_RLINEIN;
 		SYS_SetSharedPinType(SYS_PORT_D, 4, 0, 0);

 		/* IIC Configure Step without PLL: */
 		/* Add MCLK(256*Fs) in. */

 		I2S_SET_INTERNAL_CODEC(I2S, 0x08, 0x1F);	// Mute headphone of Left channel
 		I2S_SET_INTERNAL_CODEC(I2S, 0x09, 0x1F);	// Mute headphone of Right channel
 		I2S_SET_INTERNAL_CODEC(I2S, 0x10, 0x0F);	//Mute the ADC Left channel volume
 		I2S_SET_INTERNAL_CODEC(I2S, 0x11, 0x0F);	//Mute the ADC Right channel volume
 		I2S_SET_INTERNAL_CODEC(I2S, 0x12, 0x0F);	//Mute the ADC Side tone volume

 		I2S_SET_INTERNAL_CODEC(I2S, 0x02, 0xC0);	//Set CODEC slave

 		I2S_SET_INTERNAL_CODEC(I2S, 0x01, 0x80);	//Digital Part Enable
 		I2S_SET_INTERNAL_CODEC(I2S, 0x0F, 0xF0);	//Enable Analog Part
 		I2S_SET_INTERNAL_CODEC(I2S, 0x0E, 0x00);	//ADC input select Line in

 		I2S_SET_INTERNAL_CODEC(I2S, 0x0B, 0xF3);	//Analog Part Enable
		I2S_SET_INTERNAL_CODEC(I2S, 0x0D, 0x31);	//Biasing enable
 		I2S_SET_INTERNAL_CODEC(I2S, 0x0B, 0xE3);
 		for (i=0; i < 15; i++)	//Delay 1.5s~2.5s
 			CLK_SysTickDelay(100000);
 		I2S_SET_INTERNAL_CODEC(I2S, 0x0A, 0x09); // DAC output only
// 		I2S_SET_INTERNAL_CODEC(I2S, 0x0A, 0x24); // mic output only
// 		I2S_SET_INTERNAL_CODEC(I2S, 0x0B, 0xF0);
 		I2S_SET_INTERNAL_CODEC(I2S, 0x0B, 0xD0); // az
 		I2S_SET_INTERNAL_CODEC(I2S, 0x00, 0xD0);	//ADC digital enabled
 		CLK_SysTickDelay(100000);	//Delay 100mS

#if 0 // original exampe
 		I2S_SET_INTERNAL_CODEC(I2S, 0x08, 0x06);	//Un-mute Headphone and set volume
		I2S_SET_INTERNAL_CODEC(I2S, 0x09, 0x06);	//Un-mute Headphone and set volume
		I2S_SET_INTERNAL_CODEC(I2S, 0x10, 0x08);	//Un-Mute the ADC Left channel volume
		I2S_SET_INTERNAL_CODEC(I2S, 0x11, 0x08);	//Un-Mute the ADC Right channel volume
#else

		I2S_SET_INTERNAL_CODEC(I2S, 0x08, 0x00);	// Un-Mute headphone of Left channel
		I2S_SET_INTERNAL_CODEC(I2S, 0x09, 0x00);	// Un-Mute headphone of Right channel
 		I2S_SET_INTERNAL_CODEC(I2S, 0x10, 0x07);	//az Un-Mute the ADC Left channel volume
 		I2S_SET_INTERNAL_CODEC(I2S, 0x11, 0x07);	//az Un-Mute the ADC Right channel volume
//		I2S_SET_INTERNAL_CODEC(I2S, 0x10, 0x15);	//Un-Mute the ADC Left channel volume
//		I2S_SET_INTERNAL_CODEC(I2S, 0x11, 0x05);	//Un-Mute the ADC Right channel volume
#endif
 				//		I2S_SET_INTERNAL_CODEC(I2S, 0x12, 0x00);	//Un-Mute the ADC Side tone volume

     /* If Fs is changed, please Mute Headphone First and soft reset digital part after MCLK is stable. */
 }





 void I2S_Init(I2S_nuc505_instance_t *config_handle)
 {
	 uint16_t buffer_size;
	 uint16_t buff_threshold_pos;
	 buffer_size = num_of_words_in_buffer_per_chenel * num_of_bytes_in_word * 2 ;//2 for L/R
	 PcmRxBuff = (uint8_t*)malloc(buffer_size * 2);// 2 for double buffering
	 PcmTxBuff = (uint8_t*)malloc(buffer_size * 2);// 2 for double buffering

 		/* Enable I2S Module clock */
     CLK_EnableModuleClock(I2S_MODULE);
 		/* I2S module clock from APLL */
 		// APLL = 49152031Hz
     if( (48000 == config_handle->sample_rate) || (96000 == config_handle->sample_rate))
     {
 		CLK_SET_APLL(CLK_APLL_49152031);	// I2S = 49152031Hz / (0+1) for 8k, 12k, 16k, 24k, 32k, 48k, and 96k sampling rate
     }
     else if (44100 == config_handle->sample_rate)
     {
 		// APLL = 45158425Hz
 		 CLK_SET_APLL(CLK_APLL_45158425); 		// I2S = 45158425Hz / (0+1) for 11025, 22050, and 44100 sampling rate
     }
#if 	1 == CONFIG_I2S_NUC505_USE_EXTERNAL_CODEC
 		CLK_SetModuleClock(I2S_MODULE, CLK_I2S_SRC_APLL, 3);//3
#else
 		CLK_SetModuleClock(I2S_MODULE, CLK_I2S_SRC_APLL, 0);
#endif
     /* Reset IP */
     SYS_ResetModule(I2S_RST);
     /*---------------------------------------------------------------------------------------------------------*/
     /* Init I/O Multi-function                                                                                 */
     /*---------------------------------------------------------------------------------------------------------*/
     /* Configure multi-function pins for I2S */
 		// GPC[8]  = MCLK
 		// GPC[9]  = DIN
 		// GPC[10] = DOUT
 		// GPC[11] = LRCLK
 		// GPC[12] = BCLK
 		SYS->GPC_MFPH  = (SYS->GPC_MFPH & (~SYS_GPC_MFPH_PC8MFP_Msk) ) | SYS_GPC_MFPH_PC8MFP_I2S_MCLK;
 		SYS->GPC_MFPH  = (SYS->GPC_MFPH & (~SYS_GPC_MFPH_PC9MFP_Msk) ) | SYS_GPC_MFPH_PC9MFP_I2S_DIN;
 		SYS->GPC_MFPH  = (SYS->GPC_MFPH & (~SYS_GPC_MFPH_PC10MFP_Msk) ) | SYS_GPC_MFPH_PC10MFP_I2S_DOUT;
#if 	1 == CONFIG_I2S_NUC505_USE_EXTERNAL_CODEC
 		SYS->GPA_MFPH  = (SYS->GPA_MFPH & (~SYS_GPA_MFPH_PA8MFP_Msk) ) | SYS_GPA_MFPH_PA8MFP_I2S_LRCLK;
 		SYS->GPA_MFPH  = (SYS->GPA_MFPH & (~SYS_GPA_MFPH_PA9MFP_Msk) ) | SYS_GPA_MFPH_PA9MFP_I2S_BCLK;
#else
 		SYS->GPC_MFPH  = (SYS->GPC_MFPH & (~SYS_GPC_MFPH_PC11MFP_Msk) ) | SYS_GPC_MFPH_PC11MFP_I2S_LRCLK;
 		SYS->GPC_MFPH  = (SYS->GPC_MFPH & (~SYS_GPC_MFPH_PC12MFP_Msk) ) | SYS_GPC_MFPH_PC12MFP_I2S_BCLK;
#endif

#if 	1 == CONFIG_I2S_NUC505_USE_EXTERNAL_CODEC
		if (I2S_NUC505_API_MASTER_MODE == config_handle->clock_mode)
		{
			/* Master mode, 16-bit word width, stereo mode, I2S format. Set TX and RX FIFO threshold to middle value. */
			I2S_Open(I2S, I2S_MODE_MASTER, config_handle->sample_rate,  (num_of_bytes_in_word-1)<<I2S_CTL_WDWIDTH_Pos,
					I2S_STEREO, I2S_FORMAT_I2S, 0);
		}
		else
		{
		   /* Slave mode, 16-bit word width, stereo mode, I2S format. Set TX and RX FIFO threshold to middle value. */
			/* I2S peripheral clock rate is equal to PCLK1 clock rate. */
			I2S_Open(I2S, I2S_MODE_SLAVE, 0,  (num_of_bytes_in_word-1)<<I2S_CTL_WDWIDTH_Pos,
					I2S_STEREO, I2S_FORMAT_I2S, 0);
		}
#else
		/* Master mode, 16-bit word width, stereo mode, I2S format. Set TX and RX FIFO threshold to middle value. */
		/* Other sampling rate please change APLL clock setting in I2S_Init() */
		I2S_Open(I2S, I2S_MODE_MASTER, config_handle->sample_rate, (num_of_bytes_in_word-1)<<I2S_CTL_WDWIDTH_Pos,
				I2S_STEREO, I2S_FORMAT_I2S, I2S_ENABLE_INTERNAL_CODEC);

		// Open MCLK
#endif
		I2S_EnableMCLK(I2S, config_handle->sample_rate*256);

		I2S_SET_TX_TH_LEVEL(I2S, I2S_FIFO_TX_LEVEL_WORD_15);
		I2S_SET_RX_TH_LEVEL(I2S, I2S_FIFO_RX_LEVEL_WORD_16);

		buff_threshold_pos = buffer_size   ;
		PcmRxThrBuff = PcmRxBuff + buff_threshold_pos;
		PcmTxThrBuff = PcmTxBuff + buff_threshold_pos;

		I2S_SET_TXDMA_STADDR(I2S, (uint32_t) PcmTxBuff );								// Tx Start Address
		I2S_SET_TXDMA_THADDR(I2S, (uint32_t) (PcmTxThrBuff - 4) );	// Tx Threshold Address
		I2S_SET_TXDMA_EADDR( I2S, (uint32_t) (PcmTxBuff + 2*buff_threshold_pos - 4));	// Tx End Address

		I2S_SET_RXDMA_STADDR(I2S, (uint32_t) PcmRxBuff );								// Rx Start Address
		I2S_SET_RXDMA_THADDR(I2S, (uint32_t) (PcmRxThrBuff -4) );	// Rx Threshold Address
		I2S_SET_RXDMA_EADDR( I2S, (uint32_t) (PcmRxBuff + 2*buff_threshold_pos - 4));	// Rx End Address

		// Open Rx Dma Enable
		I2S_ENABLE_RXDMA(I2S);

#if 	1 == CONFIG_I2S_NUC505_USE_EXTERNAL_CODEC
#else
		demo_LineIn();
//		demo_MIC0();
//		demo_MIC1();
#endif
		// Clear Interrupt Status
		I2S_CLR_INT_FLAG(I2S, I2S_STATUS_LZCIF_Msk|I2S_STATUS_RZCIF_Msk|I2S_STATUS_TXOVIF_Msk|I2S_STATUS_TXUDIF_Msk|I2S_STATUS_RXOVIF_Msk|I2S_STATUS_RXUDIF_Msk|I2S_STATUS_TDMATIF_Msk|I2S_STATUS_TDMAEIF_Msk|I2S_STATUS_RDMATIF_Msk|I2S_STATUS_RDMAEIF_Msk);
		// Rx Enable
		I2S_ENABLE_RX(I2S);

		irq_register_interrupt(I2S_IRQn , I2S_IRQHandler);
		irq_set_priority(I2S_IRQn , OS_MAX_INTERRUPT_PRIORITY_FOR_API_CALLS );
		irq_enable_interrupt(I2S_IRQn);


//		NVIC_EnableIRQ(I2S_IRQn);
		I2S_EnableInt(I2S, (I2S_IEN_RDMATIEN_Msk|I2S_IEN_RDMAEIEN_Msk));

		DEV_IOCTL_0_PARAMS(pI2SHandle->timer_dev, IOCTL_TIMER_WRAPPER_API_RESTART_COUNTER);

 }

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        I2S_nuc505_ioctl                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t I2S_nuc505_ioctl( struct dev_desc_t *adev ,const uint8_t aIoctl_num
		, void * aIoctl_param1 , void * aIoctl_param2)
{
	I2S_nuc505_instance_t *config_handle;

	config_handle = DEV_GET_CONFIG_DATA_POINTER(adev);

	switch(aIoctl_num)
	{
		case I2S_SET_PARAMS :
			num_of_words_in_buffer_per_chenel = ((I2S_API_set_params_t*)aIoctl_param1)->num_of_words_in_buffer_per_chenel;
			num_of_bytes_in_word = ((I2S_API_set_params_t*)aIoctl_param1)->num_of_bytes_in_word;
			break;

		case IOCTL_DEVICE_START :

			clock_mode = config_handle->clock_mode;
			num_of_words_in_buffer_per_chenel = config_handle->num_of_words_in_buffer_per_chenel;
			num_of_bytes_in_word = config_handle->num_of_bytes_in_word;
			if(0 == num_of_words_in_buffer_per_chenel) return 2;

			pI2SHandle = config_handle;

			I2S_Init(config_handle);

			break;

		case I2S_ENABLE_OUTPUT_IOCTL:
			start_flag = 1;
			break;

		case I2S_GET_MEASURED_SAMPLE_RATE:
			{
				if (I2S_NUC505_API_MASTER_MODE == config_handle->clock_mode)
				{
					float measured_sample_rate;
					uint32_t timer_rate;

					DEV_IOCTL_1_PARAMS(pI2SHandle->timer_dev, IOCTL_TIMER_WRAPPER_API_GET_RATE_HZ, &timer_rate);
					measured_sample_rate = ( timer_rate * num_of_words_in_buffer_per_chenel
							* ACCUMULATION_TIMES ) / saved_accumultor_timer ;
					*(uint32_t*)aIoctl_param1 = measured_sample_rate;
				}
				else
				{
					*(uint32_t*)aIoctl_param1 = config_handle->sample_rate;
				}
			}
			break;

		case I2S_SET_OUT_VOLUME_LEVEL_DB :
			{
				int8_t volume_in_db;
				uint8_t volume_in_db_uint8;
				volume_in_db = *(int8_t*)aIoctl_param1;
				volume_in_db_uint8 = (-volume_in_db)/2;

				I2S_SET_INTERNAL_CODEC(I2S, 0x08, volume_in_db_uint8);
				I2S_SET_INTERNAL_CODEC(I2S, 0x09, volume_in_db_uint8);
			}
			break ;

		default :
			return 1;
	}
	return 0;
}


