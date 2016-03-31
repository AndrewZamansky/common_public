
INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_PRINTF)   # must be here !!


INCLUDE_DIR =  $(SW_PACKAGES_ROOT_DIR)/u_boot_shell/include
INCLUDE_DIR += $(EXTERNAL_SOURCE_ROOT_DIR)/u-boot/include

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = PRINTF.c 

ifdef CONFIG_USE_SHELL 	 
 SRC += cmd_printf_dev.c
endif

VPATH = src

include $(COMMON_CC)
