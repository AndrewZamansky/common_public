ifeq ($(findstring atmega328,$(CONFIG_SOC_TYPE)),atmega328)
    ifeq ($(findstring YES,$(CONFIG_INCLUDE_INTERNAL_SERIAL_NUMBER)),YES) 	 
	    _INCLUDE_SERIAL_NUMBER := YES
	    INCLUDE_THIS_COMPONENT := YES   # must be here !!
    endif  
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
