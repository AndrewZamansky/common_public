
INCLUDE_THIS_COMPONENT := $(CONFIG_CORTEX_M0) $(CONFIG_CORTEX_M3)
INCLUDE_THIS_COMPONENT += $(CONFIG_CORTEX_M4)
INCLUDE_THIS_COMPONENT += $(CONFIG_CORTEX_A9) $(CONFIG_CORTEX_A35)

IS_CORTEX_M := $(sort\
     $(CONFIG_CORTEX_M0) $(CONFIG_CORTEX_M3) $(CONFIG_CORTEX_M4))
ifeq ($(IS_CORTEX_M),y)

    CMSIS_VERSION :=$(patsubst "%",%,$(CONFIG_CMSIS_VERSION))
    ARM_CMSIS_PATH :=$(EXTERNAL_SOURCE_ROOT_DIR)/ARM-CMSIS_v$(CMSIS_VERSION)
    ifeq ("$(wildcard $(ARM_CMSIS_PATH))","")
        $(info !--- ARM CMSIS path $(ARM_CMSIS_PATH) dont exists )
        $(info !--- download ARM CMSIS version $(CMSIS_VERSION) and)
        $(info !--- unpack it to $(ARM_CMSIS_PATH))
        $(info !--- make sure that folders 'CMSIS' and 'Device' are)
        $(info !--- located in $(ARM_CMSIS_PATH)/  after unpacking)
        $(error )
    endif

    ifeq ("","$(filter $(ARM_CMSIS_PATH),$(EXTERNAL_SRC_DIRS))")
        EXTERNAL_SRC_DIRS := $(EXTERNAL_SRC_DIRS) $(ARM_CMSIS_PATH)
    endif

    # avoid building CMSIS startup code because we are using our:
    DUMMY := $(call ADD_TO_GLOBAL_DEFINES, __PROGRAM_START)

    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH, $(ARM_CMSIS_PATH)/CMSIS/Include)
    #in new versions of CMSIS include path is different
    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH,\
                      $(ARM_CMSIS_PATH)/CMSIS/Core/Include)
    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH,\
                 $(ARM_CMSIS_PATH)/CMSIS/DSP/Include)

    GLOBAL_LIBS_PATH += $(ARM_CMSIS_PATH)/CMSIS/Lib/GCC
    #in new versions of CMSIS lib path is different
    GLOBAL_LIBS_PATH += $(ARM_CMSIS_PATH)/CMSIS/DSP/Lib/GCC
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


SRC += src/init.c


ifeq (y,$(IS_CORTEX_M))
	CORTEX_DIR := src/Cortex_M
else ifdef CONFIG_CORTEX_A9
	CORTEX_DIR := src/Cortex_A9
	INCLUDE_DIR += src/Cortex_A9
	SRC += src/Cortex_A9/cache-pl310.c
else ifdef CONFIG_CORTEX_A35
	CORTEX_DIR := src/Cortex_A35
	INCLUDE_DIR += src/Cortex_A35
endif
SRC += $(CORTEX_DIR)/core_init.c

ifeq (y,$(USING_GCC_COMPILER))
    SRC += src/low_level_calls.c
    ifdef CONFIG_CORTEX_A35
        SRC += $(CORTEX_DIR)/v8_aarch64.s
        ifdef CONFIG_ARBEL
            $(eval $(call ADD_TO_GLOBAL_CFLAGS , -mstrict-align ))
        endif
    else ifdef CONFIG_CORTEX_M4
        LIB_LOCATION := $(ARM_CMSIS_PATH)/CMSIS/Lib/GCC
    endif
    SRC += src/SWI.c
    ifndef CONFIG_OUTPUT_TYPE_STATIC_LIBRARY
        SRC += $(CORTEX_DIR)/SWI.s
        SRC += $(CORTEX_DIR)/vectors.s
        SRC += $(CORTEX_DIR)/startup.s
    endif
else ifdef CONFIG_ARMCC
    ifndef CONFIG_OUTPUT_TYPE_STATIC_LIBRARY
        SRC += $(CORTEX_DIR)/startup_armcc.s
        SRC += $(CORTEX_DIR)/vectors_armcc.s
        SRC += $(CORTEX_DIR)/SWI_armcc.s
    endif
endif

$(eval $(call ADD_TO_GLOBAL_LIBS_PATH, $(LIB_LOCATION)))

IS_LIBRARY :=$(CONFIG_OUTPUT_TYPE_STATIC_LIBRARY)

ifdef COMMON_CC#  COMMON_CC is defined during compilation stage
IS_LIBRARY :=$(filter y,$(IS_LIBRARY))
ifeq ($(IS_LIBRARY),y)
all:
	@echo no need to compile cpu related code in library
else
    include $(COMMON_CC)
endif
endif #for ifdef COMMON_CC
