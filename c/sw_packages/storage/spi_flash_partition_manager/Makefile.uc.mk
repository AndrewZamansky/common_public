
ifeq ($(findstring YES,$(CONFIG_INCLUDE_SPI_FLASH_PARTITION_MANAGER)),YES) 	 
	DYNAMIC_COMPONENT := YES
	INCLUDE_THIS_COMPONENT := YES   # must be here !!
endif  


#INCLUDE_DIR = 

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = spi_flash_partition_manager.c 

ifeq ($(findstring YES,$(CONFIG_USE_SHELL)),YES) 	 
 SRC += cmd_spi_format.c
endif

VPATH = src


include $(COMMON_CC)
