ifeq ($(sort $(CONFIG_OUTPUT_TYPE_STATIC_LIBRARY)),y)
    ifeq ($(sort $(CONFIG_INCLUDE_UBOOT_SHELL)),y)
        INCLUDE_THIS_COMPONENT := n# not ready yet
    endif
endif

ifeq ($(MAKECMDGOALS),all)
    include $(MAKEFILES_ROOT_DIR)/_functions/usefull_functions.mk
    rwildcard=$(wildcard $1$2)\
         $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

    ALL_CMD_EXT_FILES = $(strip $(call rwildcard,$(OBJ_DIR)/,*.cmd.ext))
    ALL_CMD_FILES = $(strip $(call rwildcard,$(OBJ_DIR)/,*.cmd))

    ALL_CMD_EXT_FILE := $(AUTO_GENERATED_FILES_DIR)/all_cmd_ext.c
    ALL_CMD_EXT_FILE := $(call fix_path_if_in_windows,$(ALL_CMD_EXT_FILE))
    DUMMY := $(shell $(RM) $(ALL_CMD_EXT_FILE) 2>&1)

    ALL_CMD_FILE := $(AUTO_GENERATED_FILES_DIR)/all_cmd.c
    ALL_CMD_FILE := $(call fix_path_if_in_windows,$(ALL_CMD_FILE))
    DUMMY := $(shell $(RM) $(ALL_CMD_FILE) 2>&1)

    ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
        cat = $(shell type $(call fix_path_if_in_windows,$(1)) >> $(2))
    else
        cat = $(shell cat $(call fix_path_if_in_windows,$(1)) >> $(2))
    endif

    $(eval $(foreach d,$(ALL_CMD_EXT_FILES),$(call cat,$d,$(ALL_CMD_EXT_FILE))))
    $(eval $(foreach d,$(ALL_CMD_FILES),$(call cat,$d,$(ALL_CMD_FILE))))
endif

SRC = auto_generated_command_list.c

VPATH += src

include $(COMMON_CC)
