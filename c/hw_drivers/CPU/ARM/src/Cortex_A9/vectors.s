.section ._arm_vector_table,"ax"
.extern do_startup
.extern do_undefine_instruction_exception

/*  define default exception table as traps */
ldr pc, =do_startup
b . /*undefined_instruction*/
b . /*software_interrupt*/
b . /*prefetch_abort*/
b . /*data_abort*/
b . /*not_used*/
b . /*irq*/
b . /*fiq*/




