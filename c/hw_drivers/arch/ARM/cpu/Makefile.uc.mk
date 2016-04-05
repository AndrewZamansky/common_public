
INCLUDE_THIS_COMPONENT := $(CONFIG_CORTEX_M3) $(CONFIG_CORTEX_M4) $(CONFIG_CORTEX_A9)

ifneq ($(strip $(INCLUDE_THIS_COMPONENT) ),)
    GLOBAL_INCLUDE_DIR := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(EXTERNAL_SOURCE_ROOT_DIR)/ARM-CMSIS/CMSIS/Include)
endif




#INCLUDE_DIR =  

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  

SRC =

ifdef CONFIG_CONFIG_INCLUDE_SEMIHOSTING 	 
    SRC += semihosting.c
endif

ifdef CONFIG_GCC
    SRC += SWI.s
    SRC += low_level_calls.c
    SRC += startup.s
    SRC += vectors.s
else ifdef CONFIG_GPP	 
    SRC += SWI.s
    SRC += low_level_calls.c
    SRC += startup.s
    SRC += vectors.s
else ifdef CONFIG_ARMCC
    SRC += startup_armcc.s
    SRC += vectors_armcc.s
    SRC += SWI_armcc.s
endif


VPATH = src

ifdef CONFIG_CORTEX_M3
	VPATH += src/Cortex_M3_M4
else ifdef CONFIG_CORTEX_M4
	VPATH += src/Cortex_M3_M4
else	
	INCLUDE_DIR += src/Cortex_A9
	SRC += cache-pl310.c
	VPATH += src/Cortex_A9
endif

ifdef CONFIG_CPU_SPECIFIC_FILES_ARE_SPEAD_CRITICAL 
    SPEED_CRITICAL_FILES += SWI.s #low_level_calls.c
endif

include $(COMMON_CC)
