
ifeq ($(findstring YES,$(CONFIG_INCLUDE_RAM_DISK)),YES) 	 
	INCLUDE_THIS_COMPONENT := YES
endif  


#INCLUDE_DIR = 

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = ram_disk.c 
SRC += storage_data.c 
VPATH = src

include $(COMMON_CC)
