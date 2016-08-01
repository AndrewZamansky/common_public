/*
 * file : freeRtos_cortex_M_port.c
 *
 *
 *
 *
 *
 *
 */

#include "irq_api.h"


extern void xPortPendSVHandler(void);
extern void vPortSVCHandler(void);

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

} IRQn_Type_local;

void os_start_arch_related_components()
{
	irq_register_interrupt(SVCall_IRQn , vPortSVCHandler);
	irq_register_interrupt(PendSV_IRQn , xPortPendSVHandler);

}
