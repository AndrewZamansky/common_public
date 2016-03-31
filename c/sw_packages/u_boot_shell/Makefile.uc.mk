
INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_UBOOT_SHELL)


INCLUDE_DIR =  $(SW_PACKAGES_ROOT_DIR)/u_boot_shell/include
INCLUDE_DIR += $(EXTERNAL_SOURCE_ROOT_DIR)/u-boot/include
#DEFINES = 

ifdef CONFIG_GPP
    CFLAGS = -fpermissive
endif

#ASMFLAGS =  



SRC = command.c
SRC += cli.c
SRC += cli_simple.c
SRC += cmd_help.c

SRC += uboot.c

VPATH = src

VPATH += | $(EXTERNAL_SOURCE_ROOT_DIR)/u-boot/common

include $(COMMON_CC)
