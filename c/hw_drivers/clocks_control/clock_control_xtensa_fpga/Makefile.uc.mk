ifeq ($(sort $(XTENSA_FUSIONF1_FPGA_2)),y)
    INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_INTERNAL_CLOCK_CONTROL)
endif


#INCLUDE_DIR = 

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = clock_control_xtensa_fpga.c
VPATH = src


include $(COMMON_CC)
