#################################################################
### the following section we need to run just one time per build
ifndef COMMON_INIT_SECTION_THAT_SHOULD_RUN_ONCE
 


ifdef REDEFINE_ARM_GCC_ROOT_DIR
    $(info  gcc dir  redefined to $(REDEFINE_ARM_GCC_ROOT_DIR) )
    ifeq ("$(wildcard $(REDEFINE_ARM_GCC_ROOT_DIR))","")
        $(info gcc path $(REDEFINE_ARM_GCC_ROOT_DIR) dont exists)
        $(info to use default gcc location remove/comment REDEFINE_ARM_GCC_ROOT_DIR variable in  $(REDEFINE_ARM_GCC_ROOT_DIR)/workspace_config.mk )
        $(info you can set customized gcc path in REDEFINE_ARM_GCC_ROOT_DIR variable in $(REDEFINE_ARM_GCC_ROOT_DIR)/workspace_config.mk )
        $(error )
    else
        GCC_ROOT_DIR 	:= 	$(REDEFINE_ARM_GCC_ROOT_DIR)
    endif
else
    $(info  looking for gcc in default location)
    GCC_ROOT_DIR 	:= 	$(TOOLS_ROOT_DIR)/gcc_arm/gcc4.9.3
    ifeq ("$(wildcard $(GCC_ROOT_DIR))","")
        $(info gcc path $(GCC_ROOT_DIR) dont exists )
        $(info download gcc version 4.9.3 and unpack it to $(GCC_ROOT_DIR)  )
        $(info make sure that arm-none-eabi,bin and lib  folders is located in $(GCC_ROOT_DIR)/  after unpacking   )
        $(info you can set customized gcc path in REDEFINE_ARM_GCC_ROOT_DIR variable in $(REDEFINE_ARM_GCC_ROOT_DIR)/workspace_config.mk )
        $(error )
    endif
endif



ifndef CONFIG_OPTIMIZE_LEVEL
    CONFIG_OPTIMIZE_LEVEL :=O0
endif

GNU_COMPILATION_PREFIX	:= arm
ifdef CONFIG_TARGET_OS_LINUX
    GNU_COMPILATION_PREFIX  :=$(GNU_COMPILATION_PREFIX)-linux
else  
     GNU_COMPILATION_PREFIX  :=$(GNU_COMPILATION_PREFIX)-none
endif

ifdef CONFIG_GNUEABI
    GNU_COMPILATION_PREFIX  :=$(GNU_COMPILATION_PREFIX)-gnueabi
else ifdef CONFIG_EABI
     GNU_COMPILATION_PREFIX  :=$(GNU_COMPILATION_PREFIX)-eabi
endif

ifdef CONFIG_OPTIMISE_NONE
    CONFIG_OPTIMIZE_LEVEL := O0
else ifdef CONFIG_OPTIMISE_WITH_DEBUG
    CONFIG_OPTIMIZE_LEVEL := Og
else ifdef CONFIG_OPTIMISE_SIZE
    CONFIG_OPTIMIZE_LEVEL := Os
else ifdef CONFIG_OPTIMISE_SPEED
    CONFIG_OPTIMIZE_LEVEL := O2
else ifdef CONFIG_OPTIMISE_ULTRA_SPEED
    CONFIG_OPTIMIZE_LEVEL := O3
endif

ifdef CONFIG_CORTEX_M3
    CONFIG_CPU_TYPE := cortex-m3
else ifdef CONFIG_CORTEX_M4
    CONFIG_CPU_TYPE := cortex-m4
else ifdef CONFIG_CORTEX_A9
    CONFIG_CPU_TYPE := cortex-a9
else ifdef CONFIG_ARM_926EJ_S
    CONFIG_CPU_TYPE := arm926ej-s
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



ifdef CONFIG_CORTEX_M3	
    GLOBAL_CFLAGS += -mfloat-abi=soft
else ifdef CONFIG_CORTEX_M4	
    GLOBAL_CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
endif

 ifdef CONFIG_OPTIMISE_SIZE
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

ifdef CONFIG_CORTEX_M4	
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

