
ifeq ($(findstring YES,$(CONFIG_INCLUDE_DEV_MENAGMENT)),YES) 	 
	INCLUDE_THIS_COMPONENT := YES
endif  


#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = dev_managment.c 

ifeq ($(findstring YES,$(CONFIG_USE_SHELL)),YES) 	 

 INCLUDE_DIR =  $(SW_PACKAGES_ROOT_DIR)/u_boot_shell/include
 INCLUDE_DIR += $(EXTERNAL_SOURCE_ROOT_DIR)/u-boot/include
 SRC += cmd_set_dev_param.c
 SRC += cmd_get_dev_param.c
 DEFINES += CONFIG_USE_SHELL
endif

VPATH = src

include $(COMMON_CC)
