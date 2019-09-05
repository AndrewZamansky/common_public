####### test for existence of arm gcc compiler and  #####
####### put its directory name in GCC_ROOT_DIR     #####
SEARCHED_TOOL:=armcc
SEARCHED_DIR_VARIABLE:=ARMCC_ROOT_DIR
MANUALLY_DEFINED_DIR_VARIABLE:=REDEFINE_ARMCC_ROOT_DIR
ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
    TEST_FILE_IN_SEARCHED_DIR:=bin\armcc.exe
else ifeq ($(findstring LINUX,$(COMPILER_HOST_OS)),LINUX)
    TEST_FILE_IN_SEARCHED_DIR:=bin/armcc
endif
include $(MAKEFILES_ROOT_DIR)/_include_functions/tool_existence_check.mk
####### end of tool existence test #####

#GLOBAL_CFLAGS += -c --apcs=interwork --gnu
GLOBAL_CFLAGS += -c --apcs=interwork --split_sections --gnu

ifdef CONFIG_CORTEX_M4
    ifdef CONFIG_INCLUDE_CORTEX_M_FPU
        GLOBAL_CFLAGS += --cpu=Cortex-M4.fp
    else 	 
        GLOBAL_CFLAGS += --cpu=Cortex-M4
    endif
else
    $(info err: unsported architecture)
    $(call exit,1)
endif

ifndef CONFIG_OPTIMIZE_LEVEL
    CONFIG_OPTIMIZE_LEVEL :=O0
endif


GLOBAL_CFLAGS += -$(CONFIG_OPTIMIZE_LEVEL) -g 

#stop GLOBAL_CFLAGS calculation each time it used :
GLOBAL_CFLAGS := $(GLOBAL_CFLAGS)

#caclulating assembler flags

ifdef CONFIG_CORTEX_M4
    ifdef CONFIG_INCLUDE_CORTEX_M_FPU
        GLOBAL_ASMFLAGS += --cpu=Cortex-M4.fp
    else 	 
        GLOBAL_ASMFLAGS += --cpu=Cortex-M4
    endif
endif

GLOBAL_ASMFLAGS += -g --16 --apcs=interwork

#stop GLOBAL_ASMFLAGS calculation each time it used :
GLOBAL_ASMFLAGS := $(GLOBAL_ASMFLAGS)


CC   := $(ARMCC_ROOT_DIR)/bin/armcc --c99
CCPP := $(ARMCC_ROOT_DIR)/bin/armcc --cpp
ASM  := $(ARMCC_ROOT_DIR)/bin/armasm
CC_USE_ARGS_FROM_FILE_FLAG := --via=
