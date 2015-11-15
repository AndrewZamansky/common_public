
ifeq ($(findstring YES,$(CONFIG_INCLUDE_WIRELESS_UART)),YES) 	 
	DYNAMIC_COMPONENT := YES
	INCLUDE_THIS_COMPONENT := YES   # must be here !!
endif  


#INCLUDE_DIR = 

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = wireless_uart.c 


VPATH = src

include $(COMMON_CC)
