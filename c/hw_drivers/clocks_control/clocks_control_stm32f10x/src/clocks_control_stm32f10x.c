/*
 *
 * file :   clocks_control_stm32f10x.c
 *
 *
 *
 *
 *
 */



/********  includes *********************/
#include "_project_typedefs.h"
#include "_project_defines.h"
#include "_project_func_declarations.h"

#include "clocks_control_stm32f10x_api.h"

#include "stm32f10x_rcc.h"

/********  defines *********************/

/********  types  *********************/


/********  externals *********************/


/********  local defs *********************/



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        clocks_control_stm32f10x_init                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint32_t  clocks_control_stm32f10x_init(void)
{

	RCC_ClocksTypeDef lRCC_Clocks;


	/* Start with the clocks in their expected state. */
	RCC_DeInit();

	/* Enable HSE (high speed external clock). */
	RCC_HSEConfig( RCC_HSE_ON );

	/* Wait till HSE is ready. */
	while( RCC_GetFlagStatus( RCC_FLAG_HSERDY ) == RESET )
	{
	}

	/* 2 wait states required on the flash. */
	*( ( unsigned long * ) 0x40022000 ) = 0x02;

	/* HCLK = SYSCLK */
	RCC_HCLKConfig( RCC_SYSCLK_Div1 );

	/* PCLK2 = HCLK */
	RCC_PCLK2Config( RCC_HCLK_Div1 );

	/* PCLK1 = HCLK/2 */
	RCC_PCLK1Config( RCC_HCLK_Div2 );

	/* PLLCLK = 8MHz * 9 = 72 MHz. */
	RCC_PLLConfig( RCC_PLLSource_HSE_Div1, RCC_PLLMul_9 );

	/* Enable PLL. */
	RCC_PLLCmd( ENABLE );

	/* Wait till PLL is ready. */
	while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
	{
	}

	/* Select PLL as system clock source. */
	RCC_SYSCLKConfig( RCC_SYSCLKSource_PLLCLK );

	/* Wait till PLL is used as system clock source. */
	while( RCC_GetSYSCLKSource() != 0x08 )
	{
	}

	/* Enable GPIOA, GPIOB, GPIOC, GPIOD, GPIOE and AFIO clocks */
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |RCC_APB2Periph_GPIOC
							| RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO, ENABLE );

	/* SPI2 Periph clock enable */
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_SPI2, ENABLE );



	RCC_GetClocksFreq(&lRCC_Clocks);

    return 0;

}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        clocks_control_stm32f10x_get_cpu_clock                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint32_t clocks_control_stm32f10x_get_cpu_clock(void )
{
	RCC_ClocksTypeDef lRCC_Clocks;
	RCC_GetClocksFreq(&lRCC_Clocks);
	return lRCC_Clocks.SYSCLK_Frequency;
}
