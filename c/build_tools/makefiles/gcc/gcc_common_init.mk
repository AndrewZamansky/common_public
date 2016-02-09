
#################################################################
### the following section we need to run just one time per build
ifndef COMMON_INIT_SECTION_THAT_SHOULD_RUN_ONCE


FULL_GCC_PREFIX:=
ifeq ($(strip $(GCC_ROOT_DIR)),)
    ifeq ($(strip $(GNU_COMPILATION_PREFIX)),)
        #do nothing
    else
        FULL_GCC_PREFIX :=  $(GNU_COMPILATION_PREFIX)-
        COMPILER_INCLUDE_DIR := $(GCC_ROOT_DIR)/$(GNU_COMPILATION_PREFIX)/include
        GCC_LIB_ROOT_DIR  := $(GCC_ROOT_DIR)/$(GNU_COMPILATION_PREFIX)/lib
    endif
else
    FULL_GCC_PREFIX := $(GCC_ROOT_DIR)/bin/$(GNU_COMPILATION_PREFIX)-
    COMPILER_INCLUDE_DIR := $(GCC_ROOT_DIR)/$(GNU_COMPILATION_PREFIX)/include
    GCC_LIB_ROOT_DIR  := $(GCC_ROOT_DIR)/$(GNU_COMPILATION_PREFIX)/lib
endif

ifeq ($(findstring ARM-NONE-EABI-GCC,$(CONFIG_USE_COMPILER)),ARM-NONE-EABI-GCC) 	 
    CC   :=	$(FULL_GCC_PREFIX)gcc
    ASM  :=	$(FULL_GCC_PREFIX)gcc 
    LD   :=	$(FULL_GCC_PREFIX)gcc 
else ifeq ($(findstring ARM-NONE-EABI-G++,$(CONFIG_USE_COMPILER)),ARM-NONE-EABI-G++) 
	CC   :=	$(FULL_GCC_PREFIX)g++
    ASM  :=	$(FULL_GCC_PREFIX)g++ 
    LD   :=	$(FULL_GCC_PREFIX)g++ 
endif


ELF_TO_BIN	:=	$(FULL_GCC_PREFIX)objcopy -O binary 
DISASSEMBLER	:=	$(FULL_GCC_PREFIX)objdump -d 
ELF_TO_HEX	:=	$(FULL_GCC_PREFIX)objcopy -O ihex 


GLOBAL_DEFINES := $(GLOBAL_DEFINES) GCC


endif
### end of section that run just one time per build
#####################################################



