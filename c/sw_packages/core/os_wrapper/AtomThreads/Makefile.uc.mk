INCLUDE_THIS_COMPONENT := $(CONFIG_ATOM_THREADS)
ifdef CONFIG_ATOM_THREADS

    ATOMTHREADS_RTOS_PATH :=$(EXTERNAL_SOURCE_ROOT_DIR)/atomthreads-1.3
    ifeq ("$(wildcard $(ATOMTHREADS_RTOS_PATH))","")
        $(info  )
        $(info !--- atomthreads path $(ATOMTHREADS_RTOS_PATH) dont exists )
        $(info !--- download atomthreads version 1.3 and unpack it to $(ATOMTHREADS_RTOS_PATH)  )
        $(info !--- make sure that kernel and ports folders are located in $(ATOMTHREADS_RTOS_PATH)/  after unpacking   )
        $(info  )
        $(error )
    endif
    
    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH ,  $(ATOMTHREADS_RTOS_PATH)/kernel )
    
    ifdef CONFIG_STM8
        DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH ,  $(ATOMTHREADS_RTOS_PATH)/ports/stm8 )
    endif
    ifdef CONFIG_AVR
        DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH ,  $(ATOMTHREADS_RTOS_PATH)/ports/avr )
    endif
    
endif


#INCLUDE_DIR = 

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = os_wrapper_atomthreads.c
SRC += atomthreads_stacks.c
VPATH = src

# main atomthreads files
SRC += atomkernel.c 
SRC += atommutex.c
SRC += atomqueue.c
SRC += atomsem.c
SRC += atomtimer.c
VPATH += $(ATOMTHREADS_RTOS_PATH)/kernel

# memory management

ifdef CONFIG_STM8

    # stm8 porting
    SRC += atomport.c 
    SRC += atomport-asm-cosmic.s
    VPATH += :$(ATOMTHREADS_RTOS_PATH)/ports/stm8

endif

include $(COMMON_CC)
