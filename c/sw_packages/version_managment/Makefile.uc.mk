
ifeq ($(findstring YES,$(CONFIG_INCLUDE_VERSION_MENAGMENT)),YES) 	 
	DYNAMIC_COMPONENT := YES
	INCLUDE_THIS_COMPONENT := YES   # must be here !!
endif  


INCLUDE_DIR =  $(SW_PACKAGES_ROOT_DIR)/u_boot_shell/include
INCLUDE_DIR += $(EXTERNAL_SOURCE_ROOT_DIR)/u-boot/include

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = version_managment.c

ifeq ($(findstring YES,$(CONFIG_USE_SHELL)),YES) 	 
	SRC += cmd_get_version.c
endif

VPATH = src

include $(COMMON_CC)
