/**
 *
 * file :   NVIC.c
 *
 */

#include "_project_typedefs.h"
#include "_project_defines.h"
#include "_project_func_declarations.h"

#include "dev_management_api.h"
#include "cpu_config.h"

#include "irq_api.h"
#include "auto_init_api.h"

#include "dt_memory_layout.h"

#ifdef USE_INTERNAL_SRAM
	#define RAM_START_ADDR   MEM_BASE_ADDR(internal_sram)
#endif

#define INTERNAL_EXCEPTION_NUM   16


#define START_OF_EXTERNAL_INTERRUPT_VECTOR_TABLE    \
						(RAM_START_ADDR + (INTERNAL_EXCEPTION_NUM * 4))



#if 1 == CONFIG_CORTEX_M0
 #include "core_cm0.h"
#elif 1 == CONFIG_CORTEX_M4
 #include "core_cm4.h"
#elif 1 == CONFIG_CORTEX_M3
 #include "core_cm3.h"
#else
 #error unknown cortex-m type
#endif


#if defined(__GNUC__) && !defined(__ARMCC_VERSION)
  #define IRQ_ATTR  __attribute__((interrupt("IRQ")))
#elif defined(__ARMCC_VERSION)
  #define IRQ_ATTR  __irq
#endif

// undefined if defined before
#undef NVIC_PriorityGroup_0
#undef NVIC_PriorityGroup_1
#undef NVIC_PriorityGroup_2
#undef NVIC_PriorityGroup_3
#undef NVIC_PriorityGroup_4

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

#define NVIC_writeRegU32(addr,val)  ( (*(volatile uint32_t *)(addr)) = (val) )
#define NVIC_readRegU32(addr)       ( *(volatile uint32_t *)(addr) )




enum IRQn_Type_local {
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
};

extern IRQ_ATTR void do_software_interrupt_asm(void);

struct dev_desc_t * callback_devs[
		NUMBER_OF_NVIC_EXTERNAL_INTERRUPTS + INTERNAL_EXCEPTION_NUM] = {NULL};



/**
 * common_interrupt_handler()
 *
 * return:
 */
IRQ_ATTR void common_interrupt_handler()
{
	uint32_t curr_isr ;
	struct dev_desc_t * pdev ;

	curr_isr = __get_IPSR();
	if ( curr_isr >=
			(NUMBER_OF_NVIC_EXTERNAL_INTERRUPTS + INTERNAL_EXCEPTION_NUM))
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
#if defined(__GNUC__) && !defined(__ARMCC_VERSION)
	void IRQ_ATTR __attribute__((naked)) __attribute__((optimize("O0")))
#elif defined(__ARMCC_VERSION)
	IRQ_ATTR void
#endif
isr_hard_fault()
{
	uint32_t *stack;

#if defined(__GNUC__) && !defined(__ARMCC_VERSION)
	__asm volatile
	(
		/* bit 2 in EXC_RETURN value (in LR) shows what
		 stack was used before exception happened*/
#ifdef CONFIG_CORTEX_M0
	"mrs r0, MSP    \n"/* by default read MSP*/
	"movs r2,#0x4    \n"
	"mov r1,lr    \n"
	"tst r1,r2    \n"
	"beq exception_triggered_during_MSP    \n"
	"mrs r0, PSP    \n"
"exception_triggered_during_MSP:    \n"
#else /*for cortex-M > cortex-M0*/
		"tst lr, #4    \n"
		"ite eq        \n"
		"mrseq r0, MSP \n"
		"mrsne r0, PSP \n"
#endif
		"mov %[stack], r0 \n"
		: [stack]"=r" (stack)::"r0","memory"
	);

#elif defined(__ARMCC_VERSION)
	while(1);
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
	callback_devs[int_num + INTERNAL_EXCEPTION_NUM] = pdev;
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
	NVIC_DisableIRQ((IRQn_Type)int_num);
	NVIC_ClearPendingIRQ((IRQn_Type)int_num);
	return 0;
}


/**
 * NVIC_API_Init()
 *
 * return:
 */
void  NVIC_API_Init(void)
{
	uint16_t  number_of_external_interrupts_read_from_hw;
	int16_t  i;

	// get number of external interrupts
#ifdef  CONFIG_CORTEX_M0
	i = (NUMBER_OF_NVIC_EXTERNAL_INTERRUPTS - 1) / 32;
#else
	/*
	 * SCnSCB->ICTR   shows number of interrupts in chunks of 32
	 */
	i = SCnSCB->ICTR ;
#endif

	if ( ((NUMBER_OF_NVIC_EXTERNAL_INTERRUPTS - 1) / 32) != i )
	{
		CRITICAL_ERROR("total number of interrupts defined in HW not match to maximal interrupt number defined in project configuration");
	}

	number_of_external_interrupts_read_from_hw = 32 * (i + 1);
	while ( 0 <= i )
	{
		NVIC->ICER[i] = 0xffffffff;
		NVIC->ICPR[i] = 0xffffffff;
		i--;
	}

	for (i=-14 ; i < number_of_external_interrupts_read_from_hw ; i++)
	{
		NVIC_writeRegU32((uint32_t*)START_OF_EXTERNAL_INTERRUPT_VECTOR_TABLE +
				 i, (unsigned int)common_interrupt_handler);
	}
#if (1 == CONFIG_CORTEX_M3 ) && (__CM3_REV < 0x0201)  /* core<r2p1 */
    /*set base to start of RAM*/
	// make sure that in cortex-m3 with revision < r2p1 bit 29 is 1
    SCB->VTOR = RAM_START_ADDR & SCB_VTOR_TBLBASE_Msk;
#elif (1 == CONFIG_CORTEX_M4)
	SCB->VTOR = RAM_START_ADDR;
#endif

#if (1 == CONFIG_CORTEX_M3) || (1 == CONFIG_CORTEX_M4)
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_4);
#endif

	irq_register_interrupt(HardFault_IRQn_local, isr_hard_fault);
	irq_register_interrupt(SVCall_IRQn_local, do_software_interrupt_asm);


}


AUTO_INIT_FUNCTION(NVIC_API_Init);
