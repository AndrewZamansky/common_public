/*
 *
 * file :   NVIC.c
 *
 *
 *
 *
 *
 */



/********  includes *********************/

#include "_project.h"
#include "cpu_config.h"

#include "irq_api.h"
#include "auto_init_api.h"

//locally disable IRQn_Type defined in soc
#define IRQn_Type IRQn_Type_local

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

#if 1 == CONFIG_CORTEX_M4
 #include "core_cm4.h"
#elif 1 == CONFIG_CORTEX_M3
 #include "core_cm3.h"
#else
 #error unknown cortex-m type
#endif

#if defined(__GNUC__)
  #define IRQ_ATTR	__attribute__((interrupt("IRQ")))
#elif defined(__arm)
  #define IRQ_ATTR	__irq
#endif

//#include "stm32f10x.h"
//#include "core_cm3.h"
//#include "misc.h"

#define NVIC_PriorityGroup_0         ((uint32_t)0x7) /*!< 0 bits for pre-emption priority
                                                            4 bits for subpriority */
#define NVIC_PriorityGroup_1         ((uint32_t)0x6) /*!< 1 bits for pre-emption priority
                                                            3 bits for subpriority */
#define NVIC_PriorityGroup_2         ((uint32_t)0x5) /*!< 2 bits for pre-emption priority
                                                            2 bits for subpriority */
#define NVIC_PriorityGroup_3         ((uint32_t)0x4) /*!< 3 bits for pre-emption priority
                                                            1 bits for subpriority */
#define NVIC_PriorityGroup_4         ((uint32_t)0x3) /*!< 4 bits for pre-emption priority
                                                            0 bits for subpriority */

#define NVIC_hal_writeRegU32(addr,val)		( (*(volatile uint32_t *)(addr)) = (val) )
#define NVIC_hal_readRegU32(addr)			( *(volatile uint32_t *)(addr) )



/********  defines *********************/



/********  types  *********************/


/********  externals *********************/


/********  local defs *********************/






/*---------------------------------------------------------------------------------------------------------*/
/* Function:        irq_register_interrupt                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  int_num -                                                                              */
/*                   pIsr -                                                                                      */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine enable given interrupt                                                    */
/*---------------------------------------------------------------------------------------------------------*/
int irq_register_interrupt(int int_num , isr_t pIsr)
{

	// +16 offset becouse IRQn_Type starts from -16
    NVIC_hal_writeRegU32( RAM_START_ADDR + (((int)int_num + 16) << 2),(unsigned int)pIsr);// ( int_num  ) * 4
    return 0;
}




/*---------------------------------------------------------------------------------------------------------*/
/* Function:        NVIC_DisableInt                                                                         */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  int_num -                                                                              */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine disable given interrupt                                                   */
/*---------------------------------------------------------------------------------------------------------*/
int irq_disable_interrupt(int int_num)
{
	NVIC_DisableIRQ(int_num);
    NVIC_ClearPendingIRQ(int_num);

    return 0;
}



void  NVIC_API_Init(void)
{

	int8_t  i;



	// get number of external interrupts
	i= SCnSCB->ICTR ;
    /*
     * Disable all interrupts.
     */

    while ( i >= 0 )
	{
    	NVIC->ICER[i] = 0xffffffff;
    	NVIC->ICPR[i] = 0xffffffff;

        i--;
    }


#if (1 == CONFIG_CORTEX_M3 ) && (__CM3_REV < 0x0201)  /* core<r2p1 */
    SCB->VTOR = RAM_START_ADDR & SCB_VTOR_TBLBASE_Msk;// set base to start of RAM
#else
    // make sure that in cortex-m3 with revision < r2p1 bit 29 is 1
    SCB->VTOR = RAM_START_ADDR;
#endif

    NVIC_SetPriorityGrouping(NVIC_PriorityGroup_4);



}


#include "arch.h"

/**********  defines ************/



AUTO_INIT_FUNCTION(NVIC_API_Init);

