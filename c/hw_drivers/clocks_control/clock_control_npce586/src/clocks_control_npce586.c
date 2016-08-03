/*
 *
 * file :   clocks_control_npce586.c
 *
 *
 *
 *
 *
 */



/********  includes *********************/
#include "_project.h"

#include "auto_init_api.h"

#include "clocks_api.h"




/********  defines *********************/

/********  types  *********************/


/********  externals *********************/


/********  local defs *********************/




uint8_t xtal_set_clock(uint32_t rate)
{
	return 0;
}
uint32_t xtal_get_clock(void )
{
	return CONFIG_DT_XTAL_CLOCK_RATE;
}
clocks_common_t input_xtal_clock = {CONFIG_DT_XTAL_CLOCK_RATE , xtal_set_clock , xtal_get_clock};



uint8_t core_set_clock(uint32_t rate)
{

	//clocks_control_npce586_init();
    return 0;
}
uint32_t core_get_clock(void )
{
	return 20000000;
}
clocks_common_t core_clock = {0 , core_set_clock , core_get_clock};



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        clocks_api_init                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
void clocks_init(void)
{


    clocks_api_add_clock(CONFIG_DT_XTAL_CLOCK, &input_xtal_clock);
    clocks_api_add_clock(CONFIG_DT_CORE_CLOCK, &core_clock);


}

AUTO_INIT_FUNCTION(clocks_init);
