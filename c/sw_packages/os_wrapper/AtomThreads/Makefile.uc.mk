
ifeq ($(findstring atomthreads,$(CONFIG_INCLUDE_OS)),atomthreads) 	 
	GLOBAL_INCLUDE_DIR := $(GLOBAL_INCLUDE_DIR) $(EXTERNAL_SOURCE_ROOT_DIR)/atomthreads-1.3/kernel
	ifeq ($(findstring stm8,$(CONFIG_CPU_TYPE)),stm8) 	 
		GLOBAL_INCLUDE_DIR := $(GLOBAL_INCLUDE_DIR) $(EXTERNAL_SOURCE_ROOT_DIR)/atomthreads-1.3/ports/stm8
	else ifeq ($(findstring avr,$(CONFIG_CPU_TYPE)),avr) 	 
		GLOBAL_INCLUDE_DIR := $(GLOBAL_INCLUDE_DIR) $(EXTERNAL_SOURCE_ROOT_DIR)/atomthreads-1.3/ports/avr
	endif 
	GLOBAL_DEFINES := $(GLOBAL_DEFINES) OS_ATOMTHREADS=1
	_NO_OS_FOUND = NO
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
