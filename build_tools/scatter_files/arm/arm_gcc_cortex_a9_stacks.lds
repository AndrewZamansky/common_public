
SYS_STACK_SIZE = 128;
IRQ_STACK_SIZE = 1024;
FIQ_STACK_SIZE = 128;
SVC_STACK_SIZE = 1024;
ABT_STACK_SIZE = 128;
UND_STACK_SIZE = 128;

__irq_stack_buttom__ = . ;
. += IRQ_STACK_SIZE;
. = ALIGN (8); 
__irq_stack_top__ = . ;

__fiq_stack_buttom__ = . ;
. += FIQ_STACK_SIZE;
. = ALIGN (8);
__fiq_stack_top__ = . ;

__svc_stack_buttom__ = . ;
. += SVC_STACK_SIZE;
. = ALIGN (8);
__svc_stack_top__ = . ;

__abt_stack_buttom__ = . ;
. += ABT_STACK_SIZE;
. = ALIGN (8);
__abt_stack_top__ = . ;

__und_stack_buttom__ = . ;
. += UND_STACK_SIZE;
. = ALIGN (8);
__und_stack_top__ = . ;

__sys_stack_buttom__ = . ;
. += SYS_STACK_SIZE;
. = ALIGN (8);
__sys_stack_top__ = . ;
