/*
 *
 * uart_stm32f10x.c
 *
 *
 */

#include "irq_api.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "misc.h"

#include "uart_stm32f10x_api.h"
#include "uart_stm32f10x.h"

#include "sw_uart_wrapper_api.h"
#include "uart_api.h"


#if !defined(INTERRUPT_PRIORITY_FOR_UART)
	#error "INTERRUPT_PRIORITY_FOR_UART should be defined"
#endif

#if CHECK_INTERRUPT_PRIO_FOR_OS_SYSCALLS(INTERRUPT_PRIORITY_FOR_UART)
	#error "priority should be lower then maximal priority for os syscalls"
#endif




#define INSTANCE(hndl)	((struct UART_STM32F103x_Instance_t*)hndl)

static struct UART_STM32F103x_Instance_t *pHw_uart_pointer_to_instance[3];


/*  function : Common_UART_Isr
 *
 *
 *
 */
static void Common_UART_Isr(struct UART_STM32F103x_Instance_t *pHandle)
{
    USART_TypeDef* USARTx;
    uint8_t cChar;

    USARTx=pHandle->USARTx_Handle;

    if( USART_GetITStatus( USARTx, USART_IT_TXE ) == SET )
    {
    	DEV_CALLBACK(pHandle->callback_dev , CALLBACK_TX_DONE,(void*)1);
    }

    if( USART_GetITStatus( USARTx, USART_IT_RXNE ) == SET )
    {
		cChar = USART_ReceiveData( USARTx );
		DEV_CALLBACK(
			pHandle->callback_dev , CALLBACK_DATA_RECEIVED,  &cChar, (void*)1);
    }
}

void UART1_Isr( void )
{
	Common_UART_Isr(pHw_uart_pointer_to_instance[0]);
}

void UART2_Isr( void )
{
	Common_UART_Isr(pHw_uart_pointer_to_instance[1]);
}

void UART3_Isr( void )
{
	Common_UART_Isr(pHw_uart_pointer_to_instance[2]);
}



size_t uart_stm32f10x_pwrite(struct dev_desc_t *adev , const uint8_t *apData , size_t aLength, size_t aOffset)
{
	struct UART_STM32F103x_Instance_t *handle;
    USART_TypeDef* USARTx;

	handle = adev->handle;
    USARTx=handle->USARTx_Handle;
    USART_SendData( USARTx, *apData );
    USART_ITConfig( USARTx , USART_IT_TXE, ENABLE );
    return 1;

}

/*  function : UART_STM32F10x_Init
 *
 *
 *
 */
inline uint8_t UART_STM32F10x_Init(struct UART_STM32F103x_Instance_t *apHandle)
{
    USART_TypeDef* USARTx;
	USART_InitTypeDef USART_InitStructure;
	USART_ClockInitTypeDef USART_ClockInitStructure;
	GPIO_InitTypeDef GPIO_InitStructureRX;
	GPIO_InitTypeDef GPIO_InitStructureTX;
	GPIO_TypeDef * GPIOx;
	IRQn_Type int_num ;
	isr_t pIsr;

	if((apHandle->uart_num < 1) || (apHandle->uart_num > 3))
	{
		return 1;
	}


	switch(apHandle->uart_num)
	{
		case 1:
			USARTx=USART1;
			pIsr= UART1_Isr;
			int_num = USART1_IRQn;
			RCC_APB2PeriphClockCmd(	RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE );

			GPIO_InitStructureTX.GPIO_Pin = GPIO_Pin_9;
			GPIO_InitStructureRX.GPIO_Pin = GPIO_Pin_10;
			GPIOx = GPIOA;
			break;
		case 2:
			USARTx=USART2;
			pIsr = UART2_Isr;
			int_num = USART2_IRQn;
			RCC_APB1PeriphClockCmd(	RCC_APB1Periph_USART2 , ENABLE );

			GPIO_InitStructureTX.GPIO_Pin = GPIO_Pin_2;
			GPIO_InitStructureRX.GPIO_Pin = GPIO_Pin_3;
			GPIOx = GPIOA;
			break;
		case 3:
			USARTx=USART3;
			pIsr = UART3_Isr;
			int_num = USART3_IRQn;
			RCC_APB1PeriphClockCmd(	RCC_APB1Periph_USART3 , ENABLE );

			GPIO_InitStructureTX.GPIO_Pin = GPIO_Pin_10;
			GPIO_InitStructureRX.GPIO_Pin = GPIO_Pin_11;
			GPIOx = GPIOB;
			break;
		default :
			return 1;
	}

	pHw_uart_pointer_to_instance[apHandle->uart_num-1]=apHandle;
	/* Configure USART Rx pin as input floating */
	GPIO_InitStructureRX.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init( GPIOx, &GPIO_InitStructureRX );


	/* Configure USART Tx pin as alternate function push-pull */
	GPIO_InitStructureTX.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructureTX.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init( GPIOx, &GPIO_InitStructureTX );

	apHandle->USARTx_Handle = USARTx;
	USART_DeInit(USARTx);


	USART_InitStructure.USART_BaudRate = apHandle->baud_rate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init( USARTx, &USART_InitStructure );

	USART_ClockInitStructure.USART_Clock = USART_Clock_Disable;
	USART_ClockInitStructure.USART_CPOL = USART_CPOL_Low;
	USART_ClockInitStructure.USART_CPHA = USART_CPHA_2Edge;
	USART_ClockInitStructure.USART_LastBit = USART_LastBit_Disable;

	USART_ClockInit(USARTx, &USART_ClockInitStructure);

	irq_register_interrupt(int_num , pIsr);
	irq_set_priority(int_num , INTERRUPT_PRIORITY_FOR_UART );
	irq_enable_interrupt(int_num);

	USART_ITConfig( USARTx, USART_IT_RXNE, ENABLE );
	USART_Cmd( USARTx, ENABLE );


	return 0;
}


uint8_t uart_stm32f10x_ioctl( struct dev_desc_t *adev ,const uint8_t aIoctl_num
		, void * aIoctl_param1 , void * aIoctl_param2)
{
	struct UART_STM32F103x_Instance_t *handle;
    USART_TypeDef* USARTx;

	handle = adev->handle;
    USARTx=handle->USARTx_Handle;
	GPIO_InitTypeDef GPIO_InitStructureTX;

	switch(aIoctl_num)
	{
		case IOCTL_UART_STM32F10X_SET_UART_NUM :
			handle->uart_num = atoi((char*)aIoctl_param1);
			break;
		case IOCTL_UART_TURN_OFF :
			USART_Cmd( USARTx, DISABLE );
			GPIO_InitStructureTX.GPIO_Mode = GPIO_Mode_IN_FLOATING;
			GPIO_Init( GPIOA, &GPIO_InitStructureTX );
			break;
		case IOCTL_UART_SET_BAUD_RATE :
			handle->baud_rate = atoi((char*)aIoctl_param1);
			break;
		case IOCTL_DEVICE_START :
			UART_STM32F10x_Init(handle);
			break;
		case IOCTL_UART_DISABLE_TX :
			USART_ITConfig( USARTx, USART_IT_TXE, DISABLE );
			break;
		case IOCTL_UART_ENABLE_TX :
	//	    USART_ITConfig( USARTx , USART_IT_TXE, ENABLE );
		    break;
		case IOCTL_SET_ISR_CALLBACK_DEV:
			handle->callback_dev =(struct dev_desc_t *) aIoctl_param1;
			break;
		default :
			return 1;
	}
	return 0;
}
