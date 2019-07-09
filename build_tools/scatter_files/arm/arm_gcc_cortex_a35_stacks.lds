#define EL3_STACK_SIZE  (4096 * 4) // 4 for each cpu core

__el3_stack_buttom__ = . ;
. += EL3_STACK_SIZE;
. = ALIGN (8);
__el3_stack_top__ = . ;
