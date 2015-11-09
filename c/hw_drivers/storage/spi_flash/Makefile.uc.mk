
ifeq ($(findstring YES,$(CONFIG_INCLUDE_SPI_FLASH)),YES) 	 
	DYNAMIC_COMPONENT := YES
	INCLUDE_THIS_COMPONENT := YES   # must be here !!
endif  

#INCLUDE_DIR =  

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = spi_flash.c 


VPATH = src


include $(COMMON_CC)
