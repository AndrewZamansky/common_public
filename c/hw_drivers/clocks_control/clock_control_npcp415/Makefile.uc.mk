ifeq ($(sort $(CONFIG_NPCP415)),y)
    INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_INTERNAL_CLOCK_CONTROL)
endif


#INCLUDE_DIR = 

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = clock_control_xtensa_npcp415.c
VPATH = src


include $(COMMON_CC)
