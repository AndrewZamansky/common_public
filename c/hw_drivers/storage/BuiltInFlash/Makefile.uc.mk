
ifeq ($(findstring YES,$(CONFIG_INCLUDE_BUILT_IN_FLASH)),YES) 	 
	INCLUDE_THIS_COMPONENT := YES
endif  


#INCLUDE_DIR = 

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = BuiltInFlash.c 
SRC += storage_data.c 
VPATH = src

include $(COMMON_CC)
