// A35 has 3 exeption layers but in FW we will work only in EL3
#define NUM_OF_EL  1// number of exception levels

// we have fixed archetecture of vector tabe:
// 4 original EL : 2 if exception originated in current EL + 2 if
// exeption originated in lower EL
#define NUM_OF_ORIGINAL_EL 4
// 4 exception types for each original EL: Serror, FIQ, IRQ, Synchronous
#define NUM_OF_EXCEPTION_TYPES 4

#define NUM_OF_ENTRIES  (NUM_OF_ORIGINAL_EL * NUM_OF_EXCEPTION_TYPES)

#define ENTRY_SIZE  0x80 // 32 instructions * 4 bytes

EXCEPTION_VECTOR_AREA_SIZE  = (0x800 +
					 ( NUM_OF_EL * NUM_OF_ENTRIES * ENTRY_SIZE));
