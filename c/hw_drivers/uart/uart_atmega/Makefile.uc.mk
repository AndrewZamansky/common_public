ifeq ($(findstring atmega328,$(CONFIG_SOC_TYPE)),atmega328)
    ifeq ($(findstring YES,$(CONFIG_INCLUDE_INTERNAL_UART)),YES) 	 
	    INCLUDE_THIS_COMPONENT := YES   # must be here !!
    endif  
endif
  


#INCLUDE_DIR =  

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = uart_atmega.c

VPATH = src


include $(COMMON_CC)
