
ifeq ($(findstring YES,$(CONFIG_INCLUDE_PRINTF)),YES) 	 
	INCLUDE_THIS_COMPONENT := YES   # must be here !!
endif  


INCLUDE_DIR =  $(SW_PACKAGES_ROOT_DIR)/u_boot_shell/include
INCLUDE_DIR += $(EXTERNAL_SOURCE_ROOT_DIR)/u-boot/include

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = PRINTF.c 

ifeq ($(findstring YES,$(CONFIG_USE_SHELL)),YES) 	 
 SRC += cmd_printf_dev.c
endif

VPATH = src

include $(COMMON_CC)
