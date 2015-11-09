#################################################################
### the following section we need to run just one time per build
ifndef COMMON_INIT_SECTION_THAT_SHOULD_RUN_ONCE
 

ifdef REDEFINE_ARM_GCC_ROOT_DIR
    $(info  gcc dir  redefined to $(REDEFINE_ARM_GCC_ROOT_DIR) )
    GCC_ROOT_DIR 	:= 	$(REDEFINE_ARM_GCC_ROOT_DIR)
else
    GCC_ROOT_DIR 	:= 	$(TOOLS_ROOT_DIR)\gcc$(CONFIG_GCC_COMPILER_VERSION)
endif


ifndef CONFIG_OPTIMIZE_LEVEL
    CONFIG_OPTIMIZE_LEVEL :=O0
endif


ifeq ($(findstring ARM-NONE-EABI-GCC,$(CONFIG_USE_COMPILER)),ARM-NONE-EABI-GCC) 	 
	GNU_COMPILATION_PREFIX	:= arm-none-eabi
endif

### GLOBAL_CFLAGS calculation
GLOBAL_CFLAGS := $(GLOBAL_CFLAGS)
GLOBAL_CFLAGS += -mcpu=$(CONFIG_CPU_TYPE) -gdwarf-2 -MD 
GLOBAL_CFLAGS += -mapcs-frame -mthumb-interwork
GLOBAL_CFLAGS += -c -Wall -fdata-sections

ifeq ($(findstring cortex-m,$(CONFIG_CPU_TYPE)),cortex-m) 	 
   	GLOBAL_CFLAGS +=  -mthumb
else	
	GLOBAL_CFLAGS += -mno-unaligned-access	
endif


ifeq ($(findstring YES,$(CONFIG_POSITION_INDEPENDENT)),YES) 	 
	GLOBAL_CFLAGS += -fPIC 
endif

ifeq ($(findstring cortex-m3,$(CONFIG_CPU_TYPE)),cortex-m3) 	
else
    GLOBAL_CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
endif

ifeq ($(findstring Os,$(CONFIG_OPTIMIZE_LEVEL)),Os) 	 
	GLOBAL_CFLAGS +=  -ffunction-sections 
endif

GLOBAL_CFLAGS += -$(CONFIG_OPTIMIZE_LEVEL) -g -g3 -ggdb3 #-gstabs3

GLOBAL_CFLAGS := $(GLOBAL_CFLAGS)

### GLOBAL_ASMFLAGS calculation
GLOBAL_ASMFLAGS += -mcpu=$(CONFIG_CPU_TYPE)  -gdwarf-2   -mthumb-interwork

ifeq ($(findstring cortex-m,$(CONFIG_CPU_TYPE)),cortex-m) 	 
	GLOBAL_ASMFLAGS += -mthumb 
else	
endif

ifeq ($(findstring cortex-m3,$(CONFIG_CPU_TYPE)),cortex-m3) 	
else
	GLOBAL_ASMFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
endif

GLOBAL_ASMFLAGS += -c -x assembler-with-cpp 

GLOBAL_ASMFLAGS += -g -g3 -ggdb3 #-gstabs3

GLOBAL_ASMFLAGS := $(GLOBAL_ASMFLAGS)

#end of flags definitions






include $(MAKEFILE_DEFS_ROOT_DIR)/gcc/gcc_common_init.mk


endif
### end of section that run just one time per build
#####################################################

