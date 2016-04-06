
ifeq ($(findstring atomthreads,$(CONFIG_INCLUDE_OS)),atomthreads) 	 
    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH ,  $(EXTERNAL_SOURCE_ROOT_DIR)/atomthreads-1.3/kernel )
	ifeq ($(findstring stm8,$(CONFIG_CPU_TYPE)),stm8) 	 
        DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH ,  $(EXTERNAL_SOURCE_ROOT_DIR)/atomthreads-1.3/ports/stm8 )
	else ifeq ($(findstring avr,$(CONFIG_CPU_TYPE)),avr) 	 
        DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(EXTERNAL_SOURCE_ROOT_DIR)/atomthreads-1.3/ports/avr )
	endif 
	INCLUDE_THIS_COMPONENT := YES   # must be here !!
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
VPATH += $(EXTERNAL_SOURCE_ROOT_DIR)/atomthreads-1.3/kernel

# memory management

# stm8 porting
SRC += atomport.c 
SRC += atomport-asm-cosmic.s
VPATH += :$(EXTERNAL_SOURCE_ROOT_DIR)/atomthreads-1.3/ports/stm8

include $(COMMON_CC)
