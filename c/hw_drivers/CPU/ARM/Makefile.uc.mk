
ifeq ($(findstring cortex-m3,$(CONFIG_CPU_TYPE)),cortex-m3) 	 
    INCLUDE_THIS_COMPONENT := YES   # must be here !!
else ifeq ($(findstring cortex-m4,$(CONFIG_CPU_TYPE)),cortex-m4) 	 
    INCLUDE_THIS_COMPONENT := YES   # must be here !!
else ifeq ($(findstring cortex-a9,$(CONFIG_CPU_TYPE)),cortex-a9) 	 
    INCLUDE_THIS_COMPONENT := YES   # must be here !!
endif  

GLOBAL_INCLUDE_DIR += $(EXTERNAL_SOURCE_ROOT_DIR)/ARM-CMSIS/CMSIS/Include




#INCLUDE_DIR =  

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = semihosting.c

ifeq ($(findstring GCC,$(CONFIG_USE_COMPILER)),GCC) 	 
    SRC += SWI.s
    SRC += low_level_calls.c
    SRC += startup.s
    SRC += vectors.s
else ifeq ($(findstring ARMCC,$(CONFIG_USE_COMPILER)),ARMCC) 
    SRC += startup_armcc.s
    SRC += vectors_armcc.s
    SRC += SWI_armcc.s
endif


VPATH = src

ifeq ($(findstring cortex-m,$(CONFIG_CPU_TYPE)),cortex-m)  
	VPATH += src/Cortex_M3_M4
else	
	INCLUDE_DIR += src/Cortex_A9
	SRC += cache-pl310.c
	VPATH += src/Cortex_A9
endif

SPEED_CRITICAL_FILES += SWI.s #low_level_calls.c

include $(COMMON_CC)
