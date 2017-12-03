
INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_DEV_MENAGMENT)


#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = dev_management.c
SRC += dev_placement_with_section_bounds.c

ifdef CONFIG_INCLUDE_SHELL
    DEFINES += CONFIG_USE_SHELL
endif

VPATH = src

include $(COMMON_CC)
