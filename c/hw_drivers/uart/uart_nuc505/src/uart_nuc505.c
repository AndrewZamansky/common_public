/*
 *
 * uart_stm32f10x.c
 *
 *
 *
 *
 *
 */

/********  includes *********************/

#include "src/_uart_nuc505_prerequirements_check.h"

#include "uart_nuc505.h"


#include "NUC505Series.h"

#include "sw_uart_wrapper_api.h"
#include "NVIC_api.h"

/********  defines *********************/

#define INSTANCE(hndl)	((UART_NUC505_Instance_t*)hndl)

/********  types  *********************/


/* ---------------------------- External variables ---------------------------------*/

/* ------------------------ External functions ------------------------------*/

/* ------------------------ Exported variables ------------------------------*/


/********  local defs *********************/
UART_NUC505_Instance_t UART_NUC505_Instance;


#define UART_NUC505_NUM_OF_UARTS	3
static UART_NUC505_Instance_t *pHw_uart_pointer_to_instance[UART_NUC505_NUM_OF_UARTS];



 /*---------------------------------------------------------------------------------------------------------*/
 /* UART Callback function                                                                                  */
 /*---------------------------------------------------------------------------------------------------------*/
 void UART_TEST_HANDLE(UART_NUC505_Instance_t *pInst)
 {
     uint8_t u8InChar = 0xFF;
     uint32_t u32IntSts = UART0->INTSTS;

     if(u32IntSts & UART_INTSTS_RDAINT_Msk) {
 //        printf("\nInput:");

         /* Get all the input characters */
//         while(UART_IS_RX_READY(UART0)) {
             /* Get the character from UART Buffer */
             u8InChar = UART_READ(UART0);

     		if (NULL !=pInst->callback_dev )
     		{

				DEV_CALLBACK_2_PARAMS(pInst->callback_dev ,
						CALLBACK_DATA_RECEIVED,  &u8InChar, (void*)1);
     		}
//     		printf("%c ", u8InChar);
//
//             if(u8InChar == '0') {
//                 g_bWait = FALSE;
//             }
//
//             /* Check if buffer full */
//             if(g_u32comRbytes < RXBUFSIZE) {
//                 /* Enqueue the character */
//                 g_u8RecData[g_u32comRtail] = u8InChar;
//                 g_u32comRtail = (g_u32comRtail == (RXBUFSIZE - 1)) ? 0 : (g_u32comRtail + 1);
//                 g_u32comRbytes++;
//             }

    //     }

//         printf("\nTransmission Test:");
     }

     if(u32IntSts & UART_INTSTS_THREINT_Msk)
     {
     	if(pInst->callback_dev)
     	{
     		DEV_CALLBACK_1_PARAMS(pInst->callback_dev , CALLBACK_TX_DONE,(void*)1);
     	}
//         uint16_t tmp;
//         tmp = g_u32comRtail;
//         if(g_u32comRhead != tmp) {
//             u8InChar = g_u8RecData[g_u32comRhead];
//             UART_WRITE(UART0, u8InChar);
//             g_u32comRhead = (g_u32comRhead == (RXBUFSIZE - 1)) ? 0 : (g_u32comRhead + 1);
//             g_u32comRbytes--;
    }
}



 /*---------------------------------------------------------------------------------------------------------*/
 /* ISR to handle UART Channel 0 interrupt event                                                            */
 /*---------------------------------------------------------------------------------------------------------*/
 void __attribute__((interrupt("IRQ"))) UART0_IRQHandler(void)
 {
     UART_TEST_HANDLE(pHw_uart_pointer_to_instance[0]);
 }


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        uart_nuc505_pwrite                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
 size_t uart_nuc505_pwrite(const void *aHandle ,const uint8_t *apData , size_t aLength, size_t aOffset)
{
//	UART1_SendData8(*apData);
	UART_WRITE(UART0, *apData);
    UART_EnableInt(UART0,  UART_INTEN_THREIEN_Msk );
    //UART1_ITConfig( UART1_IT_TXE, ENABLE );

	return 1;

}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        uart_nuc505_ioctl                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t uart_nuc505_ioctl( void * const aHandle ,const uint8_t aIoctl_num
		, void * aIoctl_param1 , void * aIoctl_param2)
{
	switch(aIoctl_num)
	{
		case IOCTL_GET_PARAMS_ARRAY_FUNC :
			*(uint8_t*)aIoctl_param2 =   0; //size
			break;


		case IOCTL_UART_SET_BAUD_RATE :
			INSTANCE(aHandle)->baud_rate = *(uint32_t*)aIoctl_param1;
			break;
		case IOCTL_DEVICE_START :
			switch(INSTANCE(aHandle)->uart_num)
			{
				case 0:
					pHw_uart_pointer_to_instance[0] = (UART_NUC505_Instance_t*)aHandle;

				    /* Select IP clock source */
				    CLK_SetModuleClock(UART0_MODULE, CLK_UART0_SRC_EXT, 0);

				    /* Enable IP clock */
				    CLK_EnableModuleClock(UART0_MODULE);
				    /*---------------------------------------------------------------------------------------------------------*/
				    /* Init I/O Multi-function                                                                                 */
				    /*---------------------------------------------------------------------------------------------------------*/
				    /* Configure multi-function pins for UART0 RXD and TXD */
				    SYS->GPB_MFPL  = (SYS->GPB_MFPL & (~SYS_GPB_MFPL_PB0MFP_Msk) ) | SYS_GPB_MFPL_PB0MFP_UART0_TXD;
				    SYS->GPB_MFPL  = (SYS->GPB_MFPL & (~SYS_GPB_MFPL_PB1MFP_Msk) ) | SYS_GPB_MFPL_PB1MFP_UART0_RXD;

				    /*---------------------------------------------------------------------------------------------------------*/
				    /* Init UART                                                                                               */
				    /*---------------------------------------------------------------------------------------------------------*/
				    /* Reset UART module */
				    SYS_ResetModule(UART0_RST);

				    /* Configure UART0 and set UART0 baud rate */
				    UART_Open(UART0, INSTANCE(aHandle)->baud_rate);

				    /* Enable UART RDA/RLS/Time-out interrupt */
				    UART_EnableInt(UART0, (UART_INTEN_RDAIEN_Msk  | UART_INTEN_RXTOIEN_Msk));

					NVIC_API_RegisterInt(UART0_IRQn , UART0_IRQHandler);
					NVIC_API_SetPriority(UART0_IRQn , INTERRUPT_LOWEST_PRIORITY - 1 );
					NVIC_API_EnableInt(UART0_IRQn);
				    break;
				case 1:
					break;
				case 2:
					break;
				default :
					return 1;
			}


			break;
		case IOCTL_UART_DISABLE_TX :
			UART_DISABLE_INT(UART0,  UART_INTEN_THREIEN_Msk);
			//UART1_ITConfig( UART1_IT_TXE, DISABLE );
			break;
		case IOCTL_UART_ENABLE_TX :
		    //UART1_ITConfig( UART1_IT_TXE, ENABLE );
		    break;
		case IOCTL_SET_ISR_CALLBACK_DEV:
			INSTANCE(aHandle)->callback_dev =(pdev_descriptor) aIoctl_param1;
			break;
		default :
			return 1;
	}
	return 0;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        uart_nuc505_api_dev_descriptor                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t  uart_nuc505_api_init_dev_descriptor(pdev_descriptor aDevDescriptor)
{
	if(NULL == aDevDescriptor) return 1;


	aDevDescriptor->handle = &UART_NUC505_Instance;
	aDevDescriptor->ioctl = uart_nuc505_ioctl;
	aDevDescriptor->pwrite = uart_nuc505_pwrite;

	return 0 ;

}
