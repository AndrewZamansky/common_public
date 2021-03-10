ifeq ($(sort $(CONFIG_INCLUDE_SHELL)),y)
    INCLUDE_THIS_COMPONENT := y
endif

ifeq ($(MAKECMDGOALS),all)
    include $(MAKEFILES_ROOT_DIR)/common.mk

    rwildcard=$(wildcard $1$2)\
         $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

    ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
        cat = $(shell type $(call fix_path_if_in_windows,$(1)) >> $(2))
    else
        cat = $(shell cat $(1) >> $(2))
    endif

    #####  generate file for uboot commands  #######
    ALL_CMD_EXT_FILES = $(strip $(call rwildcard,$(OBJ_DIR)/,*.cmd.ext))
    ALL_CMD_FILES = $(strip $(call rwildcard,$(OBJ_DIR)/,*.cmd))

    ALL_CMD_EXT_FILE := $(AUTO_GENERATED_FILES_DIR)/all_cmd_ext.c
    ALL_CMD_EXT_FILE := $(call fix_path_if_in_windows,$(ALL_CMD_EXT_FILE))
    DUMMY := $(shell $(RM) $(ALL_CMD_EXT_FILE) 2>&1)

    ALL_CMD_FILE := $(AUTO_GENERATED_FILES_DIR)/all_cmd.c
    ALL_CMD_FILE := $(call fix_path_if_in_windows,$(ALL_CMD_FILE))
    DUMMY := $(shell $(RM) $(ALL_CMD_FILE) 2>&1)

    $(eval $(foreach d,$(ALL_CMD_EXT_FILES),$(call cat,$d,$(ALL_CMD_EXT_FILE))))
    $(eval $(foreach d,$(ALL_CMD_FILES),$(call cat,$d,$(ALL_CMD_FILE))))
    ##### end of generate file for uboot commands  #######


    #####  generate file for shell binary commands  #######
    ALL_BIN_CMD_EXT_FILES = $(strip $(call rwildcard,$(OBJ_DIR)/,*.bin_cmd.ext))
    ALL_BIN_CMD_FILES = $(strip $(call rwildcard,$(OBJ_DIR)/,*.bin_cmd))

    ALL_BIN_CMD_EXT_FILE :=\
             $(AUTO_GENERATED_FILES_DIR)/all_bin_commands_externals.c
    ALL_BIN_CMD_EXT_FILE :=\
               $(call fix_path_if_in_windows,$(ALL_BIN_CMD_EXT_FILE))
    DUMMY := $(shell $(RM) $(ALL_BIN_CMD_EXT_FILE) 2>&1)

    ALL_BIN_CMD_FILE := $(AUTO_GENERATED_FILES_DIR)/all_bin_commands.c
    ALL_BIN_CMD_FILE := $(call fix_path_if_in_windows,$(ALL_BIN_CMD_FILE))
    DUMMY := $(shell $(RM) $(ALL_BIN_CMD_FILE) 2>&1)

    $(eval $(foreach d,$(ALL_BIN_CMD_EXT_FILES),$(call\
                           cat,$d,$(ALL_BIN_CMD_EXT_FILE))))
    $(eval $(foreach d,$(ALL_BIN_CMD_FILES),$(call\
                               cat,$d,$(ALL_BIN_CMD_FILE))))
    ##### end of generate file for shell binary commands  #######

    CURRENT_COMPILATION_DIR_NAME := $(notdir $(abspath .))
    CMD_LIST_OBJ_FILE_DIR :=$(OBJ_DIR)/$(CURRENT_COMPILATION_DIR_NAME)
    CMD_LIST_OBJ_FILE :=$(CMD_LIST_OBJ_FILE_DIR)/auto_generated_command_list.o
    CMD_LIST_OBJ_FILE :=$(call fix_path_if_in_windows,$(CMD_LIST_OBJ_FILE))
    SHELL_OUT :=$(shell $(RM) $(CMD_LIST_OBJ_FILE) 2>&1)
endif

SRC = auto_generated_command_list.c

VPATH += src

include $(COMMON_CC)
