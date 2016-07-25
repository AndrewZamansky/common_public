ifeq ($(findstring stm32f10x,$(CONFIG_SOC_TYPE)),stm32f10x)
    ifeq ($(findstring YES,$(CONFIG_INCLUDE_INTERNAL_SERIAL_NUMBER)),YES) 	 
	    _INCLUDE_SERIAL_NUMBER := YES
	    INCLUDE_THIS_COMPONENT := YES   # must be here !!
    endif  
endif


#INCLUDE_DIR = 

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = serial_number_stm32f10x.c 


VPATH = src


include $(COMMON_CC)
