
ifeq ($(sort $(CONFIG_I94XXX) $(CONFIG_CORTEX_M0)),y)
    INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_INTERNAL_UART)
endif


#INCLUDE_DIR =  

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = uart_i9xxxx.c

VPATH = src

SRC += uart.c

ifdef CONFIG_I94XXX
    VPATH += $(I94XXX_SRC_DIR)
else ifdef CONFIG_CORTEX_M0
    VPATH += $(I96XXX_SRC_DIR)
endif

include $(COMMON_CC)
