
ifeq ($(findstring YES,$(CONFIG_INCLUDE_SPI_FLASH_PARTITION_MANAGER)),YES) 	 
	DYNAMIC_COMPONENT := YES
	INCLUDE_THIS_COMPONENT := YES   # must be here !!
endif  


#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = spi_flash_partition_manager.c 


VPATH = src


include $(COMMON_CC)
