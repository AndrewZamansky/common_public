#include "cpu_config.h"

#define STACK_SIZE_FOR_EACH_CORE  4096
#define ALL_EL3_STACK_SIZE (STACK_SIZE_FOR_EACH_CORE * CORTEX_A_NUMBER_OF_CORES)

__el3_stack_buttom__ = . ;
. += ALL_EL3_STACK_SIZE;
. = ALIGN (8);
__el3_stack_top__ = . ;
