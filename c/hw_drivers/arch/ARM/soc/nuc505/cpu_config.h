/*
 * file : cpu_config.h
 *
 *
 *
 *
 *
 *
 */

#ifndef _cpu_config_H
#define _cpu_config_H

/*  just copy from NUC505Series.h */

#define ARM_MATH_CM4	1

/*IRQn_Type can be defined in different files describing different socs .
 so it needs to be disabled in api functions */
#define IRQn_Type IRQn_Type_GPIO_NUC505_API_H_
#include "NUC505Series.h"
#undef IRQn_Type

#if 0 // defined in NUC505Series.h

/* Configuration of the Cortex-M# Processor and Core Peripherals */
#define __CM4_REV                 0x0201    /*!< Core Revision r2p1                               */
#define __NVIC_PRIO_BITS          4         /*!< Number of Bits used for Priority Levels          */
#define __Vendor_SysTickConfig    1         /*!< Set to 1 if different SysTick Config is used     */
#define __MPU_PRESENT             1         /*!< MPU present or not                               */
#define __FPU_PRESENT             1         /*!< FPU present or not                               */

typedef enum IRQn {
    /******  Cortex-M4 Processor Exceptions Numbers ***************************************************/
    NonMaskableInt_IRQn           = -14,      /*!<  2 Non Maskable Interrupt                        */
    MemoryManagement_IRQn         = -12,      /*!<  4 Memory Management Interrupt                   */
    BusFault_IRQn                 = -11,      /*!<  5 Bus Fault Interrupt                           */
    UsageFault_IRQn               = -10,      /*!<  6 Usage Fault Interrupt                         */
    SVCall_IRQn                   = -5,       /*!< 11 SV Call Interrupt                             */
    DebugMonitor_IRQn             = -4,       /*!< 12 Debug Monitor Interrupt                       */
    PendSV_IRQn                   = -2,       /*!< 14 Pend SV Interrupt                             */
    SysTick_IRQn                  = -1,       /*!< 15 System Tick Interrupt                         */

    /******  NUC505 Specific Interrupt Numbers ********************************************************/

    PWR_IRQn                      = 0,        /*!< Power On Interrupt                    						*/
    WDT_IRQn                      = 1,        /*!< Watch Dog Timer Interrupt                        */
    APU_IRQn                      = 2,        /*!< Audio Process Unit Interrupt                     */
    I2S_IRQn                      = 3,        /*!< I2S  Interrupt	*/
    EINT0_IRQn                    = 4,        /*!< External GPIO Group 0 Interrupt               									*/
    EINT1_IRQn                    = 5,        /*!< External GPIO Group 1 Interrupt  																*/
    EINT2_IRQn                    = 6,        /*!< External GPIO Group 2 Interrupt                        					*/
    EINT3_IRQn                    = 7,        /*!< External GPIO Group 3 Interrupt                       					*/
    SPIM_IRQn                     = 8,        /*!< SPI_ROM Interrupt                       					*/
    USBD_IRQn                     = 9,        /*!< USB Device 2.0 Interrupt                       	*/
    TMR0_IRQn                   	= 10,       /*!< Timer0 Interrupt                       						*/
    TMR1_IRQn                   	= 11,       /*!< Timer1 Interrupt                       					*/
    TMR2_IRQn                   	= 12,       /*!< Timer2 Interrupt                       					*/
    TMR3_IRQn                   	= 13,       /*!< Timer3 Interrupt                       					*/
    SDH_IRQn                      = 14,       /*!< SDH Interrupt                       						*/
    PWM0_IRQn                     = 15,       /*!< PWM0 Interrupt                       						*/
    PWM1_IRQn                     = 16,       /*!< PWM1 Interrupt                            				*/
    PWM2_IRQn                     = 17,       /*!< PWM2 Interrupt                            				*/
    PWM3_IRQn                     = 18,       /*!< PWM3 Interrupt                            				*/
    RTC_IRQn                      = 19,       /*!< Real Time Clock Interrupt                        */
    SPI0_IRQn                     = 20,       /*!< SPI0 Interrupt                            				*/
    I2C1_IRQn                     = 21,       /*!< I2C1 Interrupt                            				*/
    I2C0_IRQn                     = 22,       /*!< I2C0 Interrupt                            				*/
    UART0_IRQn                    = 23,       /*!< UART0 Interrupt                            			*/
    UART1_IRQn                    = 24,       /*!< UART1 Interrupt                            			*/
    ADC_IRQn                      = 25,       /*!< ADC Interrupt                            				*/
    WWDT_IRQn                     = 26,       /*!< Window Watch Dog Timer Interrupt                 */
    USBH_IRQn                     = 27,       /*!< USB Host 1.1 Interrupt                           */
    UART2_IRQn                    = 28,       /*!< UART1 Interrupt                           */
    LVD_IRQn                      = 29,       /*!< LVD Interrupt                           */
    SPI1_IRQn                     = 30,       /*!< SPI1 Interrupt                                     */
}
IRQn_Type;


#endif

#define INTERRUPT_LOWEST_PRIORITY    ((1<<__NVIC_PRIO_BITS)-1)

#endif /* */
