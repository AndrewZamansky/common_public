
INCLUDE_THIS_COMPONENT := $(CONFIG_CORTEX_M3) $(CONFIG_CORTEX_M4) $(CONFIG_CORTEX_A9)

ifneq ($(strip $(INCLUDE_THIS_COMPONENT) ),)

    ARM_CMSIS_PATH :=$(EXTERNAL_SOURCE_ROOT_DIR)/ARM-CMSIS_v3.01
    ifeq ("$(wildcard $(ARM_CMSIS_PATH))","")
        $(info !--- ARM CMSIS path $(ARM_CMSIS_PATH) dont exists )
        $(info !--- download ARM CMSIS version 3.01 and unpack it to $(ARM_CMSIS_PATH))
        $(info !--- make sure that file Version 3.01 is located in $(ARM_CMSIS_PATH)/  after unpacking)
        $(error )
    endif

    ifeq ("","$(filter $(ARM_CMSIS_PATH),$(EXTERNAL_SRC_DIRS))")
        EXTERNAL_SRC_DIRS := $(EXTERNAL_SRC_DIRS) $(ARM_CMSIS_PATH)
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

SRC :=

USING_GCC_COMPILER :=n
ifdef CONFIG_GCC
    USING_GCC_COMPILER :=y
else ifdef USING_GCC_COMPILER
    GCC_COMPILER :=y
endif

CORTEX_M3_OR_M4 :=n
ifdef CONFIG_CORTEX_M3
    CORTEX_M3_OR_M4 :=y
else ifdef CONFIG_CORTEX_M4
    CORTEX_M3_OR_M4 :=y
endif



ifeq (y,$(CORTEX_M3_OR_M4))
	CORTEX_DIR := src/Cortex_M3_M4
else ifdef CONFIG_CORTEX_A9
	CORTEX_DIR := src/Cortex_A9
	INCLUDE_DIR += src/Cortex_A9
	SRC += src/Cortex_A9/cache-pl310.c
endif

ifeq (y,$(USING_GCC_COMPILER))
    SRC += $(CORTEX_DIR)/SWI.s
    SRC += src/low_level_calls.c
    SRC += src/SWI.c
    SRC += $(CORTEX_DIR)/startup.s
    SRC += $(CORTEX_DIR)/vectors.s
else ifdef CONFIG_ARMCC
    SRC += $(CORTEX_DIR)/startup_armcc.s
    SRC += $(CORTEX_DIR)/vectors_armcc.s
    SRC += $(CORTEX_DIR)/SWI_armcc.s
endif

include $(COMMON_CC)
