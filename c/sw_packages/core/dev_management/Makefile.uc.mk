
INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_DEV_MENAGMENT)


#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = dev_management.c

ifdef CONFIG_USE_DEVICE_TREE
    SRC += dev_placement.c
endif

ifdef CONFIG_INCLUDE_SHELL
    DEFINES += CONFIG_USE_SHELL
endif

VPATH = src

include $(COMMON_CC)
