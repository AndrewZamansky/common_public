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
    ifeq ("$(wildcard $(TOOLS_ROOT_DIR)/gcc)","")
        $(info --- path  $(TOOLS_ROOT_DIR)/gcc dont exists create it )
        $(error )
    endif

    TEST_GCC_ROOT_DIR 	:= 	$(TOOLS_ROOT_DIR)/gcc/arm
    ifeq ("$(wildcard $(TEST_GCC_ROOT_DIR)*)","")
        $(info --- gcc for arm dont exists )
        GCC_NOT_FOUND :=1
    endif

    VENDOR_NAME :=
    ifneq ($(CONFIG_GCC_VENDOR_NAME),"")
       VENDOR_NAME :=$(patsubst "%",%,$(CONFIG_GCC_VENDOR_NAME))
       VENDOR_NAME :=$(VENDOR_NAME)-
    endif
    ifndef GCC_NOT_FOUND
       TEST_GCC_ROOT_DIR 	:= 	$(TOOLS_ROOT_DIR)/gcc/arm-$(VENDOR_NAME)
       ifeq ("$(wildcard $(TEST_GCC_ROOT_DIR)*)","")
           $(info --- $(TEST_GCC_ROOT_DIR)xxx dont exists )
           $(info --- (if needed you can change vendor name using menuconfig in GCC_VENDOR_NAME variable ))
           GCC_NOT_FOUND :=1
       endif
	endif
	
    OS_PREFIX :=none-
    ifdef GCC_CONFIG_TARGET_OS_LINUX  
        OS_PREFIX:=linux-
    endif
    ifndef GCC_NOT_FOUND
       TEST_GCC_ROOT_DIR 	:= 	$(TOOLS_ROOT_DIR)/gcc/arm-$(VENDOR_NAME)$(OS_PREFIX)
       ifeq ("$(wildcard $(TEST_GCC_ROOT_DIR)*)","")
           $(info --- $(TEST_GCC_ROOT_DIR)xxx dont exists )
           $(info --- (if needed you can change target OS using menuconfig in "Building System" menu ))
           GCC_NOT_FOUND :=1
       endif
	endif
    
    ABI_PREFIX :=
    ifdef CONFIG_EABI
        ABI_PREFIX:=eabi
    endif
    ifdef CONFIG_GNUEABI
        ABI_PREFIX:=gnueabi
    endif
    ifndef GCC_NOT_FOUND
       TEST_GCC_ROOT_DIR 	:= 	$(TOOLS_ROOT_DIR)/gcc/arm-$(VENDOR_NAME)$(OS_PREFIX)$(ABI_PREFIX)
       ifeq ("$(wildcard $(TEST_GCC_ROOT_DIR)*)","")
           $(info --- $(TEST_GCC_ROOT_DIR)-xxx dont exists )
           $(info --- (if needed you can change ABI type using menuconfig in "Building System" menu ))
           GCC_NOT_FOUND :=1
       endif
	endif
                
    ifndef GCC_NOT_FOUND
       TEST_GCC_ROOT_DIR 	:= 	$(TOOLS_ROOT_DIR)/gcc/arm-$(VENDOR_NAME)$(OS_PREFIX)$(ABI_PREFIX)
       GCC_ROOT_DIR :=$(lastword $(wildcard $(TEST_GCC_ROOT_DIR)-*))#take the latest gcc version
       ifeq ("$(GCC_ROOT_DIR)","")
           $(info --- gcc fdirectory should be of form : $(GCC_ROOT_DIR)-[version][revision] )
           GCC_NOT_FOUND :=1
       endif
	endif
           
    ifdef GCC_NOT_FOUND
        TEST_GCC_ROOT_DIR 	:= $(TOOLS_ROOT_DIR)/gcc/arm-$(VENDOR_NAME)$(OS_PREFIX)$(ABI_PREFIX)-[version][revision]
        $(info --- gcc path $(TEST_GCC_ROOT_DIR) dont exists )
        $(info --- download gcc (tested version is $(CONFIG_GCC_VERSION)) )
        $(info --- unpack it to $(TEST_GCC_ROOT_DIR))
        $(info --- make sure that arm-none-eabi,bin and lib  folders is located in $(TEST_GCC_ROOT_DIR)/  after unpacking   )
        $(info --- you can also set customized gcc path in REDEFINE_ARM_GCC_ROOT_DIR variable in $(REDEFINE_ARM_GCC_ROOT_DIR)/workspace_config.mk )
        $(error )
    endif
endif



ifndef CONFIG_OPTIMIZE_LEVEL
    CONFIG_OPTIMIZE_LEVEL :=O0
endif

GNU_COMPILATION_PREFIX	:=arm-$(VENDOR_NAME)$(OS_PREFIX)$(ABI_PREFIX)


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

