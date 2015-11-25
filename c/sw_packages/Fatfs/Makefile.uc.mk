
ifeq ($(findstring YES,$(CONFIG_INCLUDE_FAT_FS)),YES) 	 
	GLOBAL_INCLUDE_DIR := $(GLOBAL_INCLUDE_DIR) $(SW_PACKAGES_ROOT_DIR)/Obtainable/Fatfs/src
	INCLUDE_THIS_COMPONENT := YES   # must be here !!
endif  


INCLUDE_DIR = $(EXTERNAL_SOURCE_ROOT_DIR)/Fatfs/src

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = ff.c 
SRC += diskio.c 

VPATH =  src | $(EXTERNAL_SOURCE_ROOT_DIR)/Fatfs/src

include $(COMMON_CC)
