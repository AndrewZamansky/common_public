#include "dt_interrupts_num.h"

#define NUM_OF_EL  3 // number of exception levels

// 4 level types * 4 exeption types
#define NUM_OF_ENTRIES  (NUMBER_OF_GIC_EXTERNAL_INTERRUPTS * 4 * 4)

#define ENTRY_SIZE  0x80 // 32 instructions * 4 bytes

EXCEPTION_VECTOR_AREA_SIZE  = (0x800 +
					 ( NUM_OF_EL * NUM_OF_ENTRIES * ENTRY_SIZE));
