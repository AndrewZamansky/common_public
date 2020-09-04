GNU_COMPILATION_PREFIX	:=xtensa-lx106-elf

GCC_ROOT_DIR :=

####### test for existence of tensilica gcc compiler and  #####
####### put its directory name in GCC_ROOT_DIR     #####
SEARCHED_TOOL :=$(GNU_COMPILATION_PREFIX)-gcc
SEARCHED_DIR_VARIABLE :=GCC_ROOT_DIR
MANUALLY_DEFINED_DIR_VARIABLE :=REDEFINE_XTENSA_GCC_DIR
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
endif



### GLOBAL_CFLAGS calculation

GLOBAL_CFLAGS += -g
GLOBAL_CFLAGS += -Wpointer-arith 
GLOBAL_CFLAGS += -Werror 
GLOBAL_CFLAGS += -Wl,-EL 
GLOBAL_CFLAGS += -fno-inline-functions

ifndef CONFIG_INCLUDE_TOOLCHAIN_LIBRARIES
    GLOBAL_CFLAGS += -nostdlib
endif

GLOBAL_CFLAGS += -mlongcalls
GLOBAL_CFLAGS += -fno-builtin-printf
GLOBAL_CFLAGS += -fdata-sections


ifdef CONFIG_GCC_OPTIMISE_SIZE
    GLOBAL_CFLAGS +=  -ffunction-sections
endif


#stop GLOBAL_CFLAGS calculation each time it used:
GLOBAL_CFLAGS := $(GLOBAL_CFLAGS)

### GLOBAL_ASMFLAGS calculation
GLOBAL_ASMFLAGS += -x assembler-with-cpp

GLOBAL_ASMFLAGS += -g

GLOBAL_ASMFLAGS := $(GLOBAL_ASMFLAGS)

#end of flags definitions

DUMMY := $(call ADD_TO_GLOBAL_DEFINES , ICACHE_FLASH)

FULL_GCC_PREFIX 		:= $(GCC_ROOT_DIR)/bin/$(GNU_COMPILATION_PREFIX)-
#COMPILER_INCLUDE_DIR 	:= $(GCC_ROOT_DIR)/$(GNU_COMPILATION_PREFIX)/include
#COMPILER_INCLUDE_DIR 	:= $(ANDROID_NDK_ROOT_DIR)/platforms/android-21/arch-arm/usr/include


CC   :=	$(FULL_GCC_PREFIX)gcc -c
ASM  :=	$(FULL_GCC_PREFIX)gcc -c
LD   :=	$(FULL_GCC_PREFIX)gcc
