
ifeq ($(findstring YES,$(CONFIG_INCLUDE_UBOOT_SHELL)),YES) 	 
	CONFIG_USE_SHELL := YES	
	INCLUDE_THIS_COMPONENT := YES   # must be here !!
endif  

INCLUDE_DIR =  $(SW_PACKAGES_ROOT_DIR)/u_boot_shell/include
INCLUDE_DIR += $(EXTERNAL_SOURCE_ROOT_DIR)/u-boot/include
#DEFINES = 

ifeq ($(findstring ARM-NONE-EABI-G++,$(CONFIG_USE_COMPILER)),ARM-NONE-EABI-G++) 
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
