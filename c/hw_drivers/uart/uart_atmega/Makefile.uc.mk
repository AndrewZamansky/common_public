ifeq ($(sort $(CONFIG_ATMEGA328)),y)
    INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_INTERNAL_UART)
endif
  


#INCLUDE_DIR =

#DEFINES =

#CFLAGS =

#ASMFLAGS =



SRC = uart_atmega.c

VPATH = src


include $(COMMON_CC)
