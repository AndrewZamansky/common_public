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


#include "NUC505Series.h"

#include "I2S_nuc505_add_component.h"

/********  defines *********************/


/********  types  *********************/


/* ---------------------------- External variables ---------------------------------*/

/* ------------------------ External functions ------------------------------*/

/* ------------------------ Exported variables ------------------------------*/


/********  local defs *********************/

static I2S_NUC505_Instance_t *pI2SHandle;


uint16_t num_of_words_in_buffer_per_chenel = 0;
uint16_t num_of_uint32_in_buffer_per_chenel;
uint8_t	num_of_bytes_in_word;

int32_t *PcmRxBuff;//[2][I2S_BUFF_LEN*2] = {{0}};
//int16_t test[2][I2S_BUFF_LEN*2] = {{0}};
int32_t *PcmTxBuff;//[2][I2S_BUFF_LEN*2] = {{0}};




float volume=1;

uint8_t i2s_loopback = 0;



void __attribute__((section(".critical_text"))) I2S_IRQHandler(void)
{
	uint32_t u32I2SIntFlag;
	int32_t *pRxBuf=NULL;
	int32_t *pTxBuf=NULL;
	uint16_t i;

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
		pRxBuf = PcmRxBuff +  (num_of_uint32_in_buffer_per_chenel*2);
		pTxBuf = PcmTxBuff +  (num_of_uint32_in_buffer_per_chenel*2);
		I2S_CLR_INT_FLAG(I2S, I2S_STATUS_RDMAEIF_Msk);

		if ( pI2SHandle->start_flag == 1 )
		{
			pI2SHandle->start_flag = 0;
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
			for(i = 0 ; i < num_of_uint32_in_buffer_per_chenel ; i++)
			{
				pTxBuf[2*i] =  pRxBuf[2*i ];
				pTxBuf[2*i + 1] =  pRxBuf[2*i + 1];

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





 void I2S_Init(void)
 {
	 uint16_t buffer_size;
	 uint16_t buff_threshold_pos;
	 num_of_uint32_in_buffer_per_chenel = num_of_words_in_buffer_per_chenel * num_of_bytes_in_word / 4 ;
	 buffer_size = num_of_words_in_buffer_per_chenel * num_of_bytes_in_word * 2 ;//2 for L/R
	 PcmRxBuff = (int32_t*)malloc(buffer_size * 2);// 2 for double buffering
	 PcmTxBuff = (int32_t*)malloc(buffer_size * 2);// 2 for double buffering

 		/* Enable I2S Module clock */
     CLK_EnableModuleClock(I2S_MODULE);
 		/* I2S module clock from APLL */
 		// APLL = 49152031Hz
 		CLK_SET_APLL(CLK_APLL_49152031);
 		// I2S = 49152031Hz / (0+1) for 8k, 12k, 16k, 24k, 32k, 48k, and 96k sampling rate

 		// APLL = 45158425Hz
 		// CLK_SET_APLL(CLK_APLL_45158425);
 		// I2S = 45158425Hz / (0+1) for 11025, 22050, and 44100 sampling rate

#if 	1 == I2S_NUC505_CONFIG_USE_EXTERNAL_CODEC
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
#if 	1 == I2S_NUC505_CONFIG_USE_EXTERNAL_CODEC
 		SYS->GPA_MFPH  = (SYS->GPA_MFPH & (~SYS_GPA_MFPH_PA8MFP_Msk) ) | SYS_GPA_MFPH_PA8MFP_I2S_LRCLK;
 		SYS->GPA_MFPH  = (SYS->GPA_MFPH & (~SYS_GPA_MFPH_PA9MFP_Msk) ) | SYS_GPA_MFPH_PA9MFP_I2S_BCLK;
#else
 		SYS->GPC_MFPH  = (SYS->GPC_MFPH & (~SYS_GPC_MFPH_PC11MFP_Msk) ) | SYS_GPC_MFPH_PC11MFP_I2S_LRCLK;
 		SYS->GPC_MFPH  = (SYS->GPC_MFPH & (~SYS_GPC_MFPH_PC12MFP_Msk) ) | SYS_GPC_MFPH_PC12MFP_I2S_BCLK;
#endif

#if 	1 == I2S_NUC505_CONFIG_USE_EXTERNAL_CODEC
	#if 	1 == I2S_NUC505_CONFIG_USE_MASTER_MODE
 	    /* Master mode, 16-bit word width, stereo mode, I2S format. Set TX and RX FIFO threshold to middle value. */
 	    I2S_Open(I2S, I2S_MODE_MASTER, 48000,  (num_of_bytes_in_word-1)<<I2S_CTL_WDWIDTH_Pos,
 	    		I2S_STEREO, I2S_FORMAT_I2S, 0);
	#else
  	   /* Slave mode, 16-bit word width, stereo mode, I2S format. Set TX and RX FIFO threshold to middle value. */
  	    /* I2S peripheral clock rate is equal to PCLK1 clock rate. */
  	    I2S_Open(I2S, I2S_MODE_SLAVE, 0,  (num_of_bytes_in_word-1)<<I2S_CTL_WDWIDTH_Pos,
  	    		I2S_STEREO, I2S_FORMAT_I2S, 0);
	#endif
#else
		/* Master mode, 16-bit word width, stereo mode, I2S format. Set TX and RX FIFO threshold to middle value. */
		/* Other sampling rate please change APLL clock setting in I2S_Init() */
		I2S_Open(I2S, I2S_MODE_MASTER, 48000, (num_of_bytes_in_word-1)<<I2S_CTL_WDWIDTH_Pos,
				I2S_STEREO, I2S_FORMAT_I2S, I2S_ENABLE_INTERNAL_CODEC);

		// Open MCLK
#endif
		I2S_EnableMCLK(I2S, 48000*256);

		I2S_SET_TX_TH_LEVEL(I2S, I2S_FIFO_TX_LEVEL_WORD_15);
		I2S_SET_RX_TH_LEVEL(I2S, I2S_FIFO_RX_LEVEL_WORD_16);


		buff_threshold_pos = num_of_uint32_in_buffer_per_chenel*2   ;// *2 for L/R

		I2S_SET_TXDMA_STADDR(I2S, (uint32_t) PcmTxBuff );								// Tx Start Address
		I2S_SET_TXDMA_THADDR(I2S, (uint32_t) (PcmTxBuff + buff_threshold_pos - 1) );	// Tx Threshold Address
		I2S_SET_TXDMA_EADDR( I2S, (uint32_t) (PcmTxBuff + 2*buff_threshold_pos - 1));	// Tx End Address

		I2S_SET_RXDMA_STADDR(I2S, (uint32_t) PcmRxBuff );								// Rx Start Address
		I2S_SET_RXDMA_THADDR(I2S, (uint32_t) (PcmRxBuff + buff_threshold_pos -1) );	// Rx Threshold Address
		I2S_SET_RXDMA_EADDR( I2S, (uint32_t) (PcmRxBuff + 2*buff_threshold_pos - 1));	// Rx End Address

		// Open Rx Dma Enable
		I2S_ENABLE_RXDMA(I2S);

#if 	1 == I2S_NUC505_CONFIG_USE_EXTERNAL_CODEC
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
uint8_t I2S_nuc505_ioctl( pdev_descriptor_t apdev ,const uint8_t aIoctl_num
		, void * aIoctl_param1 , void * aIoctl_param2)
{
	I2S_NUC505_Instance_t *handle;
	float tmp;

	handle = apdev->handle;

	switch(aIoctl_num)
	{
		case I2S_SET_PARAMS :
			num_of_words_in_buffer_per_chenel = ((I2S_API_set_params_t*)aIoctl_param1)->num_of_words_in_buffer_per_chenel;
			num_of_bytes_in_word = ((I2S_API_set_params_t*)aIoctl_param1)->num_of_bytes_in_word;
			break;

		case IOCTL_DEVICE_START :

			num_of_words_in_buffer_per_chenel = handle->num_of_words_in_buffer_per_chenel;
			num_of_bytes_in_word = handle->num_of_bytes_in_word;
			if(0 == num_of_words_in_buffer_per_chenel) return 2;

			pI2SHandle = handle;

			I2S_Init();

			break;

		case I2S_SET_VOLUME_IOCTL:
			tmp = atof((char*)aIoctl_param1);
//			if(1 >= tmp)
			{
				volume = tmp;
			}
			break;

		case I2S_ENABLE_OUTPUT_IOCTL:
			handle->start_flag = 1;

			break;

		default :
			return 1;
	}
	return 0;
}


