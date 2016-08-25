#################################################################
### the following section we need to run just one time per build
ifndef COMMON_INIT_SECTION_THAT_SHOULD_RUN_ONCE


GCC_ROOT_DIR :=

ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
    GCC_ROOT_DIR := $(HOST_GCC_ROOT_DIR)
    GNU_COMPILATION_PREFIX	:= mingw32
    OUTPUT_BIN := $(OUTPUT_BIN).exe
    FIND_MINGW_PATH=$(shell echo %path%)
    ifeq ($(findstring $(HOST_GCC_ROOT_DIR)\bin,$(FIND_MINGW_PATH)),) 	 
        $(info $(FIND_MINGW_PATH))
        $(error ---- add $(HOST_GCC_ROOT_DIR)\bin to your system PATH environment)
    endif

endif

ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)

   ifdef REDEFINE_MINGW_ROOT_DIR

       $(info  mingw dir  redefined to $(REDEFINE_MINGW_ROOT_DIR) )
       ifeq ("$(wildcard $(REDEFINE_MINGW_ROOT_DIR))","")
           $(info !--- gcc path $(REDEFINE_MINGW_ROOT_DIR) dont exists)
           $(info !--- to use default gcc location remove/comment REDEFINE_MINGW_ROOT_DIR variable in  $(WORKSPACE_ROOT_DIR)/workspace_config.mk )
           $(info !--- you can set customized gcc path in REDEFINE_MINGW_ROOT_DIR variable in $(WORKSPACE_ROOT_DIR)/workspace_config.mk )
           $(error )
       else
           GCC_ROOT_DIR 	:= 	$(REDEFINE_MINGW_ROOT_DIR)
       endif

   else #for 'ifdef REDEFINE_MINGW_ROOT_DIR'

       $(info  looking for gcc in default location)
       ifeq ("$(wildcard $(TOOLS_ROOT_DIR)/gcc)","")
           $(info !--- path  $(TOOLS_ROOT_DIR)/gcc dont exists create it )
           $(error )
       endif

       TEST_GCC_ROOT_DIR 	:= 	$(TOOLS_ROOT_DIR)/gcc/MinGW
       ifeq ("$(wildcard $(TEST_GCC_ROOT_DIR)*)","")
           $(info --- MinGW dont exists )
           GCC_NOT_FOUND :=1
       endif

       GCC_VERSION :=$(patsubst "%",%,$(CONFIG_GCC_VERSION))
       ifndef GCC_NOT_FOUND
          TEST_GCC_ROOT_DIR 	:= 	$(TOOLS_ROOT_DIR)/gcc/MinGW-$(GCC_VERSION)
          ifeq ("$(wildcard $(TEST_GCC_ROOT_DIR))","")
              $(info !--- $(TEST_GCC_ROOT_DIR) dont exists )
              $(info !--- (if needed you can change gcc version using menuconfig in "Building System" menu ))
              GCC_NOT_FOUND :=1
          endif
       endif
      
      
       ifdef GCC_NOT_FOUND
           TEST_GCC_ROOT_DIR 	:= 	$(TOOLS_ROOT_DIR)/gcc/MinGW-$(GCC_VERSION)
           $(info !--- gcc path $(TEST_GCC_ROOT_DIR) dont exists )
           $(info !--- download gcc (tested version is $(CONFIG_GCC_VERSION)) )
           $(info !--- unpack it to $(TEST_GCC_ROOT_DIR))
           $(info !--- make sure that mingw32 ,bin and lib  folders is located in $(TEST_GCC_ROOT_DIR)/  after unpacking   )
           $(info !--- you can also set customized gcc path in REDEFINE_MINGW_ROOT_DIR variable in $(WORKSPACE_ROOT_DIR)/workspace_config.mk )
           $(error )
       endif
    
       GCC_ROOT_DIR :=$(lastword $(wildcard $(TEST_GCC_ROOT_DIR)))#take the latest gcc version
    
    endif

endif #end of 'ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)'

LINKER_OUTPUT :=  $(OUTPUT_BIN)




# define flags for asm compiler :
#GLOBAL_ASMFLAGS := $(GLOBAL_ASMFLAGS)

# define flags for linker :
GLOBAL_LDFLAGS := $(GLOBAL_PROJECT_SPECIFIC_LDFLAGS)
#GLOBAL_LDFLAGS := $(GLOBAL_LDFLAGS) -mcpu=$(CONFIG_CPU_TYPE) -Wl,--gc-sections -nostartfiles  -specs=nano.specs # -msoft-float -mfloat-abi=soft  
GLOBAL_LDFLAGS := $(GLOBAL_LDFLAGS) -Wl,-Map=$(OUT_DIR)/$(OUTPUT_APP_NAME).map   # -msoft-float -mfloat-abi=soft  



#end of flags definitions





include $(MAKEFILE_DEFS_ROOT_DIR)/gcc/gcc_common_init.mk


LIBS := $(patsubst lib%,-l%,$(GLOBAL_LIBS))
LIBS := $(patsubst %.a,%,$(LIBS))
LIBRARIES_DIRS := $(patsubst %,-L%,$(GLOBAL_LIBS_PATH))


endif
### end of section that run just one time per build
#####################################################
