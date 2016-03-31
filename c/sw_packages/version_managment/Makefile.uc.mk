
INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_VERSION_MENAGMENT)
DYNAMIC_COMPONENT := $(CONFIG_INCLUDE_VERSION_MENAGMENT)


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
