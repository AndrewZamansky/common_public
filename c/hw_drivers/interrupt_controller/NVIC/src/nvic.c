/**
 *
 * file :   NVIC.c
 *
 */



/********  includes *********************/

#include "_project_typedefs.h"
#include "_project_defines.h"
#include "_project_func_declarations.h"

#include "dev_management_api.h"
#include "cpu_config.h"

#include "irq_api.h"
#include "auto_init_api.h"

#include "_nvic_prerequirements_check.h"

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

/*!< 0 bits for pre-emption priority 4 bits for subpriority */
#define NVIC_PriorityGroup_0         ((uint32_t)0x7)
/*!< 1 bits for pre-emption priority 3 bits for subpriority */
#define NVIC_PriorityGroup_1         ((uint32_t)0x6)
/*!< 2 bits for pre-emption priority 2 bits for subpriority */
#define NVIC_PriorityGroup_2         ((uint32_t)0x5)
/*!< 3 bits for pre-emption priority 1 bits for subpriority */
#define NVIC_PriorityGroup_3         ((uint32_t)0x4
/*!< 4 bits for pre-emption priority 0 bits for subpriority */
#define NVIC_PriorityGroup_4         ((uint32_t)0x3)

#define NVIC_writeRegU32(addr,val)	( (*(volatile uint32_t *)(addr)) = (val) )
#define NVIC_readRegU32(addr)		( *(volatile uint32_t *)(addr) )

static int16_t total_number_of_external_interrupts;

/********  types  *********************/
//locally disable IRQn_Type defined in soc
#define IRQn_Type IRQn_Type_local

typedef enum IRQn_local {
    /******  Cortex-M4 Processor Exceptions Numbers ********/
    NonMaskableInt_IRQn_local    = -14, /*!<  2 Non Maskable Interrupt      */
    HardFault_IRQn_local         = -13, /*!<  2 Hard Fault Interrupt      */
    MemoryManagement_IRQn_local  = -12, /*!<  4 Memory Management Interrupt */
    BusFault_IRQn_local          = -11, /*!<  5 Bus Fault Interrupt         */
    UsageFault_IRQn_local        = -10, /*!<  6 Usage Fault Interrupt       */
    SVCall_IRQn_local            = -5,  /*!< 11 SV Call Interrupt           */
    DebugMonitor_IRQn_local      = -4,  /*!< 12 Debug Monitor Interrupt     */
    PendSV_IRQn_local            = -2,  /*!< 14 Pend SV Interrupt           */
    SysTick_IRQn_local           = -1,  /*!< 15 System Tick Interrupt       */

} IRQn_Type_local;

/********  externals *********************/


/********  local variables *********************/
struct dev_desc_t * callback_devs[
                         CONFIG_DT_NUMBER_OF_EXTERNAL_INTERRUPTS + 16] = {NULL};



/**
 * common_interrupt_handler()
 *
 * return:
 */
void  IRQ_ATTR common_interrupt_handler()
{
	uint32_t curr_isr ;
	struct dev_desc_t * pdev ;

	curr_isr = __get_IPSR();
	if ( (CONFIG_DT_NUMBER_OF_EXTERNAL_INTERRUPTS + 16) <= curr_isr )
	{
		CRITICAL_ERROR("received interrupt is larger then maximal interrupt number defined in project configuration");
	}
	pdev = callback_devs[curr_isr];
	if (NULL != pdev)
	{
		DEV_CALLBACK_0_PARAMS( pdev, CALLBACK_INTERRUPT_ARRIVED) ;
		return ;
	}
	CRITICAL_ERROR("received interrupt was not registerd yet");
}


static volatile uint32_t R0_r, R1_r, R2_r, R3_r, R12_r, LR_r, PC_r, PSR_r;
/**
 * isr_hard_fault()
 *
 * return:
 */
void __attribute__((interrupt("IRQ"))) __attribute__((naked)) isr_hard_fault()
{
	uint32_t *stack;

#if defined(__GNUC__)
	register int sp asm ("sp");
	stack = (uint32_t *)sp ;
#elif defined(__arm)
	__asm
	{
		//TODO
	};
#endif

	R0_r = *stack++;
	R1_r = *stack++;
	R2_r = *stack++;
	R3_r = *stack++;
	R12_r = *stack++;
	LR_r = *stack++;
	PC_r = *stack++;
	PSR_r = *stack++;
    while (1);
}


/**
 * irq_register_interrupt()
 * @int_num  : interrupt number RELETIVE to -16 .
 *             for example if UART interrupt is next after sysTick interrupt
 *             then   int_num = 0 ( and not 16)
 *
 * return:
 */
int irq_register_interrupt(int int_num, isr_t pIsr)
{

	// +16 offset becouse IRQn_Type starts from -16
    NVIC_writeRegU32( (uint32_t*)START_OF_EXTERNAL_INTERRUPT_VECTOR_TABLE +
		int_num, (unsigned int)pIsr);// ( int_num  ) * 4
    return 0;
}


/**
 * irq_register_device_on_interrupt()
 * @int_num  : interrupt number RELETIVE to -16 .
 *             for example if UART interrupt is next after sysTick interrupt
 *             then   int_num = 0 ( and not 16)
 *
 * return:
 */
int irq_register_device_on_interrupt(int int_num, struct dev_desc_t * pdev)
{
	callback_devs[int_num + 16] = pdev;
    return  irq_register_interrupt(int_num, common_interrupt_handler);
}



/**
 * irq_disable_interrupt()
 * @int_num  : interrupt number RELETIVE to -16 .
 *             for example if UART interrupt is next after sysTick interrupt
 *             then   int_num = 0 ( and not 16)
 *
 * return:
 */
int irq_disable_interrupt(int int_num)
{
	NVIC_DisableIRQ(int_num);
    NVIC_ClearPendingIRQ(int_num);

    return 0;
}


/**
 * NVIC_API_Init()
 *
 * return:
 */
void  NVIC_API_Init(void)
{

	int16_t  i;

	// get number of external interrupts
	i = SCnSCB->ICTR ;
    /*
     * Disable all interrupts.
     */

	/*
	 * SCnSCB->ICTR   shows number of interrupts in chunks of 32
	 */
	if ( (CONFIG_DT_NUMBER_OF_EXTERNAL_INTERRUPTS / 32) != i )
	{
		CRITICAL_ERROR("total number of interrupts defined in HW not match to maximal interrupt number defined in project configuration");
	}

    total_number_of_external_interrupts = 32 * (i+1);
    while ( 0 <= i )
	{
    	NVIC->ICER[i] = 0xffffffff;
    	NVIC->ICPR[i] = 0xffffffff;
    	i--;
    }

    for (i=-14 ; i < total_number_of_external_interrupts ; i++)
    {
    	NVIC_writeRegU32((uint32_t*)START_OF_EXTERNAL_INTERRUPT_VECTOR_TABLE +
    			 i, (unsigned int)common_interrupt_handler);
    }
#if (1 == CONFIG_CORTEX_M3 ) && (__CM3_REV < 0x0201)  /* core<r2p1 */
    /*set base to start of RAM*/
    SCB->VTOR = RAM_START_ADDR & SCB_VTOR_TBLBASE_Msk;
#else
    // make sure that in cortex-m3 with revision < r2p1 bit 29 is 1
    SCB->VTOR = RAM_START_ADDR;
#endif

    NVIC_SetPriorityGrouping(NVIC_PriorityGroup_4);

    irq_register_interrupt(HardFault_IRQn_local, isr_hard_fault);


}

AUTO_INIT_FUNCTION(NVIC_API_Init);
