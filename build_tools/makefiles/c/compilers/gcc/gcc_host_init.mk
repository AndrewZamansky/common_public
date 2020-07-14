GCC_ROOT_DIR :=


ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)

   ifdef REDEFINE_MINGW_ROOT_DIR

       $(info  mingw dir  redefined to $(REDEFINE_MINGW_ROOT_DIR) )
       ifeq ("$(wildcard $(REDEFINE_MINGW_ROOT_DIR))","")
           $(info err: gcc path $(REDEFINE_MINGW_ROOT_DIR) dont exists)
           $(info ---: to use default gcc location remove/comment REDEFINE_MINGW_ROOT_DIR variable in  $(PARENT_OF_COMMON_PUBLIC_DIR)/workspace_config.mk )
           $(info ---: you can set customized gcc path in REDEFINE_MINGW_ROOT_DIR variable in $(PARENT_OF_COMMON_PUBLIC_DIR)/workspace_config.mk )
           $(call exit,1)
       else
           GCC_ROOT_DIR 	:= 	$(REDEFINE_MINGW_ROOT_DIR)
       endif

   else #for 'ifdef REDEFINE_MINGW_ROOT_DIR'

       $(info  looking for gcc in default location)
       ifeq ("$(wildcard $(TOOLS_ROOT_DIR)/gcc)","")
           $(info err: path  $(TOOLS_ROOT_DIR)/gcc dont exists create it )
           $(call exit,1)
       endif

       TEST_GCC_ROOT_DIR 	:= 	$(TOOLS_ROOT_DIR)/gcc/MinGW
       ifeq ("$(wildcard $(TEST_GCC_ROOT_DIR)*)","")
           $(info err: MinGW dont exists )
           GCC_NOT_FOUND :=1
       endif

       GCC_VERSION :=$(patsubst "%",%,$(CONFIG_MINGW_GCC_VERSION))
       ifndef GCC_NOT_FOUND
          TEST_GCC_ROOT_DIR 	:= 	$(TOOLS_ROOT_DIR)/gcc/MinGW-$(GCC_VERSION)
          ifeq ("$(wildcard $(TEST_GCC_ROOT_DIR))","")
              $(info err: $(TEST_GCC_ROOT_DIR) dont exists)
              $(info ---: (if needed you can change gcc version using)
              $(info ---: menuconfig in "Building System" menu))
              GCC_NOT_FOUND :=1
          endif
       endif
      
      
       ifdef GCC_NOT_FOUND
           TEST_GCC_ROOT_DIR 	:= 	$(TOOLS_ROOT_DIR)/gcc/MinGW-$(GCC_VERSION)
           $(info err: gcc path $(TEST_GCC_ROOT_DIR) does not exists)
           $(info ---: download gcc (tested version is $(CONFIG_GCC_VERSION)))
           $(info ---: unpack it to $(TEST_GCC_ROOT_DIR))
           $(info ---: make sure that mingw32 ,bin and lib  folders is)
           $(info ---: located in $(TEST_GCC_ROOT_DIR)/  after unpacking)
           $(info ---: you can also set customized gcc path in REDEFINE_MINGW_ROOT_DIR)
           $(info ---: variable in $(PARENT_OF_COMMON_PUBLIC_DIR)/workspace_config.mk)
           $(call exit,1)
       endif

       # take the latest gcc version:
       GCC_ROOT_DIR :=$(lastword $(wildcard $(TEST_GCC_ROOT_DIR)))

    endif

    #clear PATH environment variable to get rid of different mingw conflicts
    FULL_GCC_PREFIX :=$(call fix_path_if_in_windows,\
                   set "PATH=$(TEST_GCC_ROOT_DIR)/bin" & $(GCC_ROOT_DIR)/bin/)
    COMPILER_INCLUDE_DIR := $(GCC_ROOT_DIR)/include
    GCC_LIB_ROOT_DIR := $(GCC_ROOT_DIR)/lib

    ifdef CONFIG_MIN_GW_GCC
        COMPILE_WITH_GCC :=y
    else ifdef CONFIG_MIN_GW_GPP
        COMPILE_WITH_GPP :=y
    endif

else # for 'ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)'

    ifdef CONFIG_LINUX_HOST_GCC
        COMPILE_WITH_GCC   :=y
    else ifdef CONFIG_LINUX_HOST_GCC
        COMPILE_WITH_GPP   :=y
    endif


endif #end of 'ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)'

### GLOBAL_CFLAGS calculation
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

GLOBAL_CFLAGS += -$(CONFIG_OPTIMIZE_LEVEL) -g -g3 -ggdb3 #-gstabs3


# define flags for asm compiler :
#GLOBAL_ASMFLAGS := $(GLOBAL_ASMFLAGS)




#end of flags definitions




ifdef COMPILE_WITH_GCC
    CC   :=	$(FULL_GCC_PREFIX)gcc -c
    CCPP := $(FULL_GCC_PREFIX)gcc -c
    ASM  :=	$(FULL_GCC_PREFIX)gcc -c
    LD   :=	$(FULL_GCC_PREFIX)gcc
else ifdef COMPILE_WITH_GPP
	CC   :=	$(FULL_GCC_PREFIX)g++ -c
    CCPP := $(FULL_GCC_PREFIX)gcc -c
    ASM  :=	$(FULL_GCC_PREFIX)g++ -c
    LD   :=	$(FULL_GCC_PREFIX)g++
endif
STRIP   :=	$(FULL_GCC_PREFIX)strip
