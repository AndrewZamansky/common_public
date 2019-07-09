#define NUM_OF_EL  3 // number of exception levels
#define NUM_OF_ENTRIES  16 // 4 level types * 4 exeption types
#define ENTRY_SIZE  0x80 // 32 instructions * 4 bytes

EXCEPTION_VECTOR_AREA_SIZE  = (0x800 +
					 ( NUM_OF_EL * NUM_OF_ENTRIES * ENTRY_SIZE));
