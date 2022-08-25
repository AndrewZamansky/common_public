ifeq ($(sort $(CONFIG_ATMEGA328)),y)
    INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_INTERNAL_SERIAL_NUMBER) 
endif


#INCLUDE_DIR = 

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = serial_number_atmega.c 

ifdef CONFIG_INCLUDE_UBOOT_SHELL
	SRC += cmd_get_serial.c
endif

VPATH = src


include $(COMMON_CC)
