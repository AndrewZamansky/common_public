
ifeq ($(findstring YES,$(CONFIG_INCLUDE_CONFIG)),YES) 	 
	INCLUDE_THIS_COMPONENT := YES
endif  


INCLUDE_DIR = $(AUTO_GENERATED_FILES_DIR)

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = config.c 
SRC += included_modules.c
SRC += cmd_brd_name.c 
SRC += cmd_brd_ip.c 
SRC += cmd_cat.c 

VPATH = src : $(AUTO_GENERATED_FILES_DIR)

include $(COMMON_CC)
