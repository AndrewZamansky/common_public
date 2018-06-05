
INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_VERSION_MENAGMENT)


#INCLUDE_DIR = 

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  

# force this component to be compiled always to get updated version
CURRENT_COMPILATION_DIR_NAME := $(notdir $(abspath .))
CURR_OBJ_DIR := $(OBJ_DIR)/$(CURRENT_COMPILATION_DIR_NAME)
ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
    CURR_OBJ_DIR :=$(subst /,\,$(CURR_OBJ_DIR))
endif
SHELL_OUT :=$(shell $(RM) $(CURR_OBJ_DIR) 2>&1)


DEFINES += VERSION_STR="$(FULL_VERSION_STR)"


SRC = version_management.c


VPATH = src

include $(COMMON_CC)
