#################################################################
### the following section we need to run just one time per build
ifndef COMMON_INIT_SECTION_THAT_SHOULD_RUN_ONCE
 

HEXAGON_ROOT_DIR :=

####### test for existence of microsoft compiler and put its directory name in MSVC_ROOT_DIR #####
SEARCHED_TOOL:=hexagon-clang
SEARCHED_DIR_VARIABLE:=HEXAGON_ROOT_DIR
MANUALLY_DEFINED_DIR_VARIABLE:=REDEFINE_HEXAGON_ROOT_DIR
ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
    TEST_FILE_IN_SEARCHED_DIR:=bin\hexagon-clang.exe
else ifeq ($(findstring LINUX,$(COMPILER_HOST_OS)),LINUX)
    TEST_FILE_IN_SEARCHED_DIR:=bin/hexagon-clang
endif
include $(MAKEFILES_ROOT_DIR)/_common_include_functions/tool_existence_check.mk
####### end of tool existence test #####


ifndef CONFIG_OPTIMIZE_LEVEL
    CONFIG_OPTIMIZE_LEVEL :=O0
endif



ifdef CONFIG_GCC_OPTIMIZE_NONE
    CONFIG_OPTIMIZE_LEVEL := O0
else ifdef CONFIG_GCC_OPTIMIZE_SIZE
    CONFIG_OPTIMIZE_LEVEL := Os
else ifdef CONFIG_GCC_OPTIMIZE_SPEED
    CONFIG_OPTIMIZE_LEVEL := O2
else ifdef CONFIG_GCC_OPTIMIZE_ULTRA_SPEED
    CONFIG_OPTIMIZE_LEVEL := O3
endif



### GLOBAL_CFLAGS calculation

ifdef CONFIG_HEXAGON_VERSION_60
    GLOBAL_CFLAGS += -mv60
else
    $(error unknown hexagon version)
endif
ifdef CONFIG_COMPILE_FOR_DEBUG
    GLOBAL_CFLAGS += -g
    GLOBAL_CFLAGS += -v
else
    $(error TODO)
endif
GLOBAL_CFLAGS += -G0
GLOBAL_CFLAGS += -Wall 
GLOBAL_CFLAGS += -Werror 
GLOBAL_CFLAGS += -Wno-cast-align 
GLOBAL_CFLAGS += -Wpointer-arith 
GLOBAL_CFLAGS += -Wno-missing-braces 
GLOBAL_CFLAGS += -Wno-strict-aliasing  
GLOBAL_CFLAGS += -fno-exceptions 
GLOBAL_CFLAGS += -fno-strict-aliasing 
GLOBAL_CFLAGS += -fno-zero-initialized-in-bss 
GLOBAL_CFLAGS += -fdata-sections 
GLOBAL_CFLAGS += -mllvm 
GLOBAL_CFLAGS += -disable-hsdr 
GLOBAL_CFLAGS += -fpic 



ifdef CONFIG_GCC_OPTIMIZE_SIZE
    GLOBAL_CFLAGS +=  -ffunction-sections
endif

#stop GLOBAL_CFLAGS calculation each time it used:
GLOBAL_CFLAGS := $(GLOBAL_CFLAGS) 

### GLOBAL_ASMFLAGS calculation
GLOBAL_ASMFLAGS += 


#end of flags definitions


#DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(HEXAGON_ROOT_DIR)/.... )
CLANG_LIB_ROOT_DIR  := $(HEXAGON_ROOT_DIR)/lib

LD   :=	$(HEXAGON_ROOT_DIR)/bin/hexagon-clang.exe
CC   :=	$(HEXAGON_ROOT_DIR)/bin/hexagon-clang.exe -c

endif
### end of section that run just one time per build
#####################################################

