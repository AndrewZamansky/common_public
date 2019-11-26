ifeq ($(sort $(CONFIG_OUTPUT_TYPE_STATIC_LIBRARY)),y)
    ifeq ($(sort $(CONFIG_INCLUDE_UBOOT_SHELL)),y)
        INCLUDE_THIS_COMPONENT := y
    endif
endif

SRC = auto_generated_command_list.c

VPATH += src

include $(COMMON_CC)
