/*
 * file : freeRtos_cortex_M_port.c
 *
 *
 */

#include "irq_api.h"


extern void xPortPendSVHandler(void);
extern void vPortSVCHandler(void);

//locally disable IRQn_Type defined in soc
#define IRQn_Type IRQn_Type_local

typedef enum IRQn_local {
    /******  Cortex-M4 Processor Exceptions Numbers *************************/
    NonMaskableInt_IRQn_local      = -14, /*!<  2 Non Maskable Interrupt     */
    MemoryManagement_IRQn_local    = -12, /*!<  4 Memory Management Interrupt*/
    BusFault_IRQn_local            = -11, /*!<  5 Bus Fault Interrupt        */
    UsageFault_IRQn_local          = -10, /*!<  6 Usage Fault Interrupt      */
    SVCall_IRQn_local              = -5,  /*!< 11 SV Call Interrupt          */
    DebugMonitor_IRQn_local        = -4,  /*!< 12 Debug Monitor Interrupt    */
    PendSV_IRQn_local              = -2,  /*!< 14 Pend SV Interrupt          */
    SysTick_IRQn_local             = -1   /*!< 15 System Tick Interrupt      */

} IRQn_Type_local;


void os_start_arch_related_components()
{
	irq_register_interrupt(PendSV_IRQn_local , xPortPendSVHandler);
}
