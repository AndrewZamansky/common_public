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
#include "_nvic_prerequirements_check.h"
#include "dev_management_api.h"
#include "cpu_config.h"

#include "irq_api.h"
#include "auto_init_api.h"

#define START_OF_EXTERNAL_INTERRUPT_VECTOR_TABLE 		(RAM_START_ADDR + 64)



#if 1 == CONFIG_CORTEX_M4
 #include "core_cm4.h"
#elif 1 == CONFIG_CORTEX_M3
 #include "core_cm3.h"
#else
 #error unknown cortex-m type
#endif

/********  defines *********************/

#if defined(__GNUC__)
  #define IRQ_ATTR	__attribute__((interrupt("IRQ")))
#elif defined(__arm)
  #define IRQ_ATTR	__irq
#endif


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

#define NVIC_writeRegU32(addr,val)		( (*(volatile uint32_t *)(addr)) = (val) )
#define NVIC_readRegU32(addr)			( *(volatile uint32_t *)(addr) )

static int16_t total_number_of_external_interrupts;

/********  types  *********************/
//locally disable IRQn_Type defined in soc
#define IRQn_Type IRQn_Type_local

typedef enum IRQn_local {
    /******  Cortex-M4 Processor Exceptions Numbers ***************************************************/
    NonMaskableInt_IRQn_local           = -14,      /*!<  2 Non Maskable Interrupt                        */
    MemoryManagement_IRQn_local         = -12,      /*!<  4 Memory Management Interrupt                   */
    BusFault_IRQn_local                 = -11,      /*!<  5 Bus Fault Interrupt                           */
    UsageFault_IRQn_local               = -10,      /*!<  6 Usage Fault Interrupt                         */
    SVCall_IRQn_local                   = -5,       /*!< 11 SV Call Interrupt                             */
    DebugMonitor_IRQn_local             = -4,       /*!< 12 Debug Monitor Interrupt                       */
    PendSV_IRQn_local                   = -2,       /*!< 14 Pend SV Interrupt                             */
    SysTick_IRQn_local                  = -1,       /*!< 15 System Tick Interrupt                         */

} IRQn_Type_local;

/********  externals *********************/


/********  local variables *********************/
pdev_descriptor_t callback_devs[TOTAL_NUMBER_OF_EXTERNAL_INTERRUPS + 16] = {NULL};



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
    NVIC_writeRegU32( (uint32_t*)START_OF_EXTERNAL_INTERRUPT_VECTOR_TABLE + int_num ,(unsigned int)pIsr);// ( int_num  ) * 4
    return 0;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        common_interrupt_handler                                                                         */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                           */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                               */
/*---------------------------------------------------------------------------------------------------------*/
void  IRQ_ATTR common_interrupt_handler()
{
	uint32_t curr_isr ;
	pdev_descriptor_t pdev ;

	curr_isr = __get_IPSR();
	pdev = callback_devs[curr_isr];
	if(NULL != pdev)
	{
		DEV_CALLBACK_0_PARAMS( pdev , CALLBACK_INTERRUPT_ARRIVED) ;
		return ;
	}
	while(1);
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        irq_register_device_on_interrupt                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  int_num -                                                                              */
/*                   pdev -                                                                                      */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
int irq_register_device_on_interrupt(int int_num , pdev_descriptor_t pdev)
{
	NVIC_writeRegU32( (uint32_t*)START_OF_EXTERNAL_INTERRUPT_VECTOR_TABLE + int_num ,
			(unsigned int)common_interrupt_handler);
	callback_devs[int_num + 16] = pdev;
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

	int16_t  i;

	// get number of external interrupts
	i = SCnSCB->ICTR ;
    /*
     * Disable all interrupts.
     */

    total_number_of_external_interrupts = 32 * (i+1);
    while ( 0 <= i)
	{
    	NVIC->ICER[i] = 0xffffffff;
    	NVIC->ICPR[i] = 0xffffffff;
    	i--;
    }

    for(i=-14 ; i < total_number_of_external_interrupts ; i++)
    {
    	 NVIC_writeRegU32( (uint32_t*)START_OF_EXTERNAL_INTERRUPT_VECTOR_TABLE + i ,(unsigned int)common_interrupt_handler);
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
