
INCLUDE_THIS_COMPONENT := $(CONFIG_CORTEX_M3) $(CONFIG_CORTEX_M4) $(CONFIG_CORTEX_A9)

ifneq ($(strip $(INCLUDE_THIS_COMPONENT) ),)

    ARM_CMSIS_PATH :=$(EXTERNAL_SOURCE_ROOT_DIR)/ARM-CMSIS_v3.01
    ifeq ("$(wildcard $(ARM_CMSIS_PATH))","")
        $(info !--- ARM CMSIS path $(ARM_CMSIS_PATH) dont exists )
        $(info !--- download ARM CMSIS version 3.01 and unpack it to $(ARM_CMSIS_PATH)  )
        $(info !--- make sure that file Version 3.01 is located in $(ARM_CMSIS_PATH)/  after unpacking   )
        $(error )
    endif

    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(ARM_CMSIS_PATH)/CMSIS/Include)
endif


#INCLUDE_DIR =  

DEFINES :=
ifdef CONFIG_CODE_LOCATION_INTERNAL_SRAM
    DEFINES += CODE_LOCATION_INTERNAL_SRAM_FOR_ASM
endif
ifdef CONFIG_CODE_LOCATION_INTERNAL_ROM
    DEFINES += CODE_LOCATION_INTERNAL_ROM_FOR_ASM
endif


#CFLAGS = 

#ASMFLAGS =  

SRC =


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
else ifdef CONFIG_CORTEX_A9
	INCLUDE_DIR += src/Cortex_A9
	SRC += cache-pl310.c
	VPATH += src/Cortex_A9
endif



include $(COMMON_CC)
