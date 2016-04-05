
ifeq ($(findstring YES,$(CONFIG_INCLUDE_FAT_FS)),YES) 	 
	INCLUDE_THIS_COMPONENT := YES   # must be here !!
    GLOBAL_INCLUDE_DIR := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(EXTERNAL_SOURCE_ROOT_DIR)/Fatfs/src )
endif  



#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = ff.c 
SRC += diskio.c 

VPATH =  src | $(EXTERNAL_SOURCE_ROOT_DIR)/Fatfs/src

include $(COMMON_CC)
