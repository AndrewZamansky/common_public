VENDOR_NAME :=
ifneq ($(CONFIG_GCC_VENDOR_NAME),"")
   VENDOR_NAME :=$(patsubst "%",%,$(CONFIG_GCC_VENDOR_NAME))
   VENDOR_NAME :=$(VENDOR_NAME)-
endif
OS_PREFIX :=none-
ifdef CONFIG_GCC_TARGET_OS_LINUX
    OS_PREFIX:=linux-
endif
ABI_PREFIX :=
ifdef CONFIG_EABI
    ABI_PREFIX:=eabi
endif
ifdef CONFIG_GNUEABI
    ABI_PREFIX:=gnueabi
endif

ifdef CONFIG_CORTEX_A35
  GNU_COMPILATION_PREFIX :=aarch64-elf
else
  GNU_COMPILATION_PREFIX :=arm-$(VENDOR_NAME)$(OS_PREFIX)$(ABI_PREFIX)
endif

GCC_ROOT_DIR :=

####### test for existence of arm gcc compiler and  #####
####### put its directory name in GCC_ROOT_DIR     #####
SEARCHED_TOOL:=$(GNU_COMPILATION_PREFIX)-gcc
SEARCHED_DIR_VARIABLE:=GCC_ROOT_DIR
ifdef CONFIG_CORTEX_A35
    MANUALLY_DEFINED_DIR_VARIABLE:=REDEFINE_ARM_GCC_AARCH64_ROOT_DIR
else
    MANUALLY_DEFINED_DIR_VARIABLE:=REDEFINE_ARM_GCC_ROOT_DIR
endif
ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
    TEST_FILE_IN_SEARCHED_DIR:=bin\$(GNU_COMPILATION_PREFIX)-gcc.exe
else ifeq ($(findstring LINUX,$(COMPILER_HOST_OS)),LINUX)
    TEST_FILE_IN_SEARCHED_DIR:=bin/$(GNU_COMPILATION_PREFIX)-gcc
endif
include $(MAKEFILES_ROOT_DIR)/_include_functions/tool_existence_check.mk
####### end of tool existence test #####


ifndef CONFIG_OPTIMIZE_LEVEL
    CONFIG_OPTIMIZE_LEVEL :=O0
endif



ifdef CONFIG_GCC_OPTIMISE_NONE
    CONFIG_OPTIMIZE_LEVEL := O0
else ifdef CONFIG_GCC_OPTIMISE_WITH_DEBUG
    CONFIG_OPTIMIZE_LEVEL := Og
else ifdef CONFIG_GCC_OPTIMISE_SIZE
    CONFIG_OPTIMIZE_LEVEL := Os
else ifdef CONFIG_GCC_OPTIMISE_SPEED
    CONFIG_OPTIMIZE_LEVEL := O2
else ifdef CONFIG_GCC_OPTIMISE_ULTRA_SPEED
    CONFIG_OPTIMIZE_LEVEL := O3
    #CONFIG_OPTIMIZE_LEVEL := Ofast
endif

ifdef CONFIG_CORTEX_M0
    CONFIG_CPU_TYPE := cortex-m0
else ifdef CONFIG_CORTEX_M3
    CONFIG_CPU_TYPE := cortex-m3
else ifdef CONFIG_CORTEX_M4
    CONFIG_CPU_TYPE := cortex-m4
else ifdef CONFIG_CORTEX_A9
    CONFIG_CPU_TYPE := cortex-a9
else ifdef CONFIG_CORTEX_A35
    CONFIG_CPU_TYPE := cortex-a35
else ifdef CONFIG_ARM_926EJ_S
    CONFIG_CPU_TYPE := arm926ej-s
endif


### GLOBAL_CFLAGS calculation

GLOBAL_CFLAGS += -mcpu=$(CONFIG_CPU_TYPE) -gdwarf-2 -MD
ifndef CONFIG_CORTEX_A35
    GLOBAL_CFLAGS += -mapcs-frame -mthumb-interwork
endif
GLOBAL_CFLAGS += -Wall -fdata-sections

ifeq ($(findstring cortex-m,$(CONFIG_CPU_TYPE)),cortex-m)
   	GLOBAL_CFLAGS +=  -mthumb
else	
    ifndef CONFIG_CORTEX_A35
        GLOBAL_CFLAGS += -mno-unaligned-access
    endif
endif


ifdef CONFIG_CORTEX_M0
    GLOBAL_CFLAGS += -mfloat-abi=soft
else ifdef CONFIG_CORTEX_M3
    GLOBAL_CFLAGS += -mfloat-abi=soft
else ifdef CONFIG_CORTEX_M4
    ifdef CONFIG_INCLUDE_CORTEX_M_FPU
        GLOBAL_CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
    else
        GLOBAL_CFLAGS += -mfloat-abi=soft
    endif
else ifdef CONFIG_CORTEX_A35
    ifdef CONFIG_DISABLE_ARM_FPU
        GLOBAL_CFLAGS += -march=armv8-a+nofp
    else
        GLOBAL_CFLAGS += -march=armv8-a
    endif
endif

ifdef CONFIG_GCC_OPTIMISE_SIZE
    GLOBAL_CFLAGS +=  -ffunction-sections
endif

GLOBAL_CFLAGS += -$(CONFIG_OPTIMIZE_LEVEL) -g -g3 -ggdb3 #-gstabs3


#stop GLOBAL_CFLAGS and GLOBAL_CPPFLAGS calculation each time it used :
GLOBAL_CPPFLAGS := $(GLOBAL_CFLAGS)
GLOBAL_CFLAGS := -std=c99 $(GLOBAL_CFLAGS)

### GLOBAL_ASMFLAGS calculation
GLOBAL_ASMFLAGS += -mcpu=$(CONFIG_CPU_TYPE) -gdwarf-2
ifndef CONFIG_CORTEX_A35
    GLOBAL_ASMFLAGS += -mthumb-interwork
endif

ifeq ($(findstring cortex-m,$(CONFIG_CPU_TYPE)),cortex-m)
	GLOBAL_ASMFLAGS += -mthumb 
else	
endif


GLOBAL_ASMFLAGS += -x assembler-with-cpp

GLOBAL_ASMFLAGS += -g -g3 -ggdb3 #-gstabs3

GLOBAL_ASMFLAGS := $(GLOBAL_ASMFLAGS)

#end of flags definitions



FULL_GCC_PREFIX      := $(GCC_ROOT_DIR)/bin/$(GNU_COMPILATION_PREFIX)-
COMPILER_INCLUDE_DIR := $(GCC_ROOT_DIR)/$(GNU_COMPILATION_PREFIX)/include
GCC_LIB_ROOT_DIR     := $(GCC_ROOT_DIR)/$(GNU_COMPILATION_PREFIX)/lib

ifdef CONFIG_GCC
    CC   := $(FULL_GCC_PREFIX)gcc -c
    CCPP := $(FULL_GCC_PREFIX)gcc -c
    ASM  := $(FULL_GCC_PREFIX)gcc -c
    LD   := $(FULL_GCC_PREFIX)gcc
else ifdef CONFIG_GPP
	CC   := $(FULL_GCC_PREFIX)g++ -c
	CCPP := $(FULL_GCC_PREFIX)g++ -c
    ASM  := $(FULL_GCC_PREFIX)g++ -c
    LD   := $(FULL_GCC_PREFIX)g++
endif
CC_USE_ARGS_FROM_FILE_FLAG := @
