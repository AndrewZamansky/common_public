ifeq ($(sort $(CONFIG_XTENSA_HIFI3_NTCA)),y)
    INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_INTERNAL_CLOCK_CONTROL)
endif


#INCLUDE_DIR = 

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = clock_control_i96xxx_hifi3.c
VPATH = src


include $(COMMON_CC)