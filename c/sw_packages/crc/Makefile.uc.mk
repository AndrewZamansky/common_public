
ifeq ($(findstring YES,$(CONFIG_INCLUDE_CRC)),YES) 	 
	INCLUDE_THIS_COMPONENT := YES   # must be here !!
endif  


#INCLUDE_DIR = 

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = crc32.c


VPATH = src

include $(COMMON_CC)
