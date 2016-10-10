/*
 * file : irq_definitions.h
 *
 *
 *
 *
 *
 *
 */

#ifndef _irq_definitions_H
#define _irq_definitions_H

#include "_project.h"

#ifndef TRAP_IRQHandler
	#define TRAP_IRQHandler NonHandledInterrupt
#else
	INTERRUPT void TRAP_IRQHandler(void);
#endif

#ifndef TLI_IRQHandler
	#define TLI_IRQHandler NonHandledInterrupt
#else
	INTERRUPT void TLI_IRQHandler(void);
#endif

#ifndef AWU_IRQHandler
	#define AWU_IRQHandler NonHandledInterrupt
#else
	INTERRUPT void AWU_IRQHandler(void);
#endif

#ifndef CLK_IRQHandler
	#define CLK_IRQHandler NonHandledInterrupt
#else
	INTERRUPT void CLK_IRQHandler(void);
#endif

#ifndef EXTI_PORTA_IRQHandler
	#define EXTI_PORTA_IRQHandler NonHandledInterrupt
#else
	INTERRUPT void EXTI_PORTA_IRQHandler(void);
#endif

#ifndef EXTI_PORTB_IRQHandler
	#define EXTI_PORTB_IRQHandler NonHandledInterrupt
#else
	INTERRUPT void EXTI_PORTB_IRQHandler(void);
#endif

#ifndef EXTI_PORTC_IRQHandler
	#define EXTI_PORTC_IRQHandler NonHandledInterrupt
#else
	INTERRUPT void EXTI_PORTC_IRQHandler(void);
#endif

#ifndef EXTI_PORTD_IRQHandler
	#define EXTI_PORTD_IRQHandler NonHandledInterrupt
#else
	INTERRUPT void EXTI_PORTD_IRQHandler(void);
#endif

#ifndef EXTI_PORTE_IRQHandler
	#define EXTI_PORTE_IRQHandler NonHandledInterrupt
#else
	INTERRUPT void EXTI_PORTE_IRQHandler(void);
#endif

#ifndef SPI_IRQHandler
	#define SPI_IRQHandler NonHandledInterrupt
#else
	INTERRUPT void SPI_IRQHandler(void);
#endif

#ifndef CONFIG_INCLUDE_INTERNAL_TIMER
	#define TIM1_UPD_OVF_TRG_BRK_IRQHandler NonHandledInterrupt
#else
	INTERRUPT void TIM1_UPD_OVF_TRG_BRK_IRQHandler(void);
#endif

#ifndef TIM1_CAP_COM_IRQHandler
	#define TIM1_CAP_COM_IRQHandler NonHandledInterrupt
#else
	INTERRUPT void TIM1_CAP_COM_IRQHandler(void);
#endif

#ifndef TIM2_UPD_OVF_BRK_IRQHandler
	#define TIM2_UPD_OVF_BRK_IRQHandler NonHandledInterrupt
#else
	INTERRUPT void TIM2_UPD_OVF_BRK_IRQHandler(void);
#endif

#ifndef TIM2_CAP_COM_IRQHandler
	#define TIM2_CAP_COM_IRQHandler NonHandledInterrupt
#else
	INTERRUPT void TIM2_CAP_COM_IRQHandler(void);
#endif

#ifndef CONFIG_INCLUDE_INTERNAL_UART
	#define UART1_TX_IRQHandler NonHandledInterrupt
#else
	INTERRUPT void UART1_TX_IRQHandler(void);
#endif

#ifndef CONFIG_INCLUDE_INTERNAL_UART
	#define UART1_RX_IRQHandler NonHandledInterrupt
#else
	INTERRUPT void UART1_RX_IRQHandler(void);
#endif

#ifndef I2C_IRQHandler
	#define I2C_IRQHandler NonHandledInterrupt
#else
	INTERRUPT void I2C_IRQHandler(void);
#endif

#ifndef ADC1_IRQHandler
	#define ADC1_IRQHandler NonHandledInterrupt
#else
	INTERRUPT void ADC1_IRQHandler(void);
#endif

#ifndef TIM4_UPD_OVF_IRQHandler
	#define TIM4_UPD_OVF_IRQHandler NonHandledInterrupt
#else
	INTERRUPT void TIM4_UPD_OVF_IRQHandler(void);
#endif

#ifndef EEPROM_EEC_IRQHandler
	#define EEPROM_EEC_IRQHandler NonHandledInterrupt
#else
	INTERRUPT void EEPROM_EEC_IRQHandler(void);
#endif


#else
#pragma message( "warning : this header file had already been included" )
#endif /* */
