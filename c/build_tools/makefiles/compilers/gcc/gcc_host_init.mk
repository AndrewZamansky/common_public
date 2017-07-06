#################################################################
### the following section we need to run just one time per build
ifndef COMMON_INIT_SECTION_THAT_SHOULD_RUN_ONCE


GCC_ROOT_DIR :=


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
           $(info !--- MinGW dont exists )
           GCC_NOT_FOUND :=1
       endif

       GCC_VERSION :=$(patsubst "%",%,$(CONFIG_MINGW_GCC_VERSION))
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

    #clear PATH environment variable to get rid of different mingw conflicts
    FULL_GCC_PREFIX 		:= set "PATH=$(TEST_GCC_ROOT_DIR)/bin" & $(GCC_ROOT_DIR)/bin/
    FULL_GCC_PREFIX := $(subst /,\,$(FULL_GCC_PREFIX))
    COMPILER_INCLUDE_DIR 	:= $(GCC_ROOT_DIR)/include
    GCC_LIB_ROOT_DIR  		:= $(GCC_ROOT_DIR)/lib

    ifdef CONFIG_MIN_GW_GCC
        COMPILE_WITH_GCC   :=y
    else ifdef CONFIG_MIN_GW_GPP
        COMPILE_WITH_GPP   :=y
    endif

else # for 'ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)'

    ifdef CONFIG_LINUX_HOST_GCC
        COMPILE_WITH_GCC   :=y
    else ifdef CONFIG_LINUX_HOST_GCC
        COMPILE_WITH_GPP   :=y
    endif


endif #end of 'ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)'

### GLOBAL_CFLAGS calculation



# define flags for asm compiler :
#GLOBAL_ASMFLAGS := $(GLOBAL_ASMFLAGS)




#end of flags definitions




ifdef COMPILE_WITH_GCC
    CC   :=	$(FULL_GCC_PREFIX)gcc -c
    ASM  :=	$(FULL_GCC_PREFIX)gcc -c
    LD   :=	$(FULL_GCC_PREFIX)gcc
else ifdef COMPILE_WITH_GPP
	CC   :=	$(FULL_GCC_PREFIX)g++ -c
    ASM  :=	$(FULL_GCC_PREFIX)g++ -c
    LD   :=	$(FULL_GCC_PREFIX)g++
endif
STRIP   :=	$(FULL_GCC_PREFIX)strip



endif
### end of section that run just one time per build
#####################################################
