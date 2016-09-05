
ifdef CONFIG_NUC505
    INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_INTERNAL_UART)
endif


#INCLUDE_DIR =  

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = uart_nuc505.c

VPATH = src

SRC += uart.c
SRC += sys.c

VPATH += | $(NUC505_SRC_DIR)

include $(COMMON_CC)
