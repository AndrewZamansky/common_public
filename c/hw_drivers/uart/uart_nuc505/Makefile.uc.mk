
ifeq ($(sort $(CONFIG_NUC505)),y)
    INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_INTERNAL_UART)
endif


#INCLUDE_DIR =  

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = uart_nuc505.c

VPATH = src

SRC += uart.c

VPATH += | $(NUC505_SRC_DIR)

include $(COMMON_CC)
