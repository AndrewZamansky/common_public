
INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_ERROR_HANDLING)


#INCLUDE_DIR = 

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  

SRC = errors.c

ifdef CONFIG_INCLUDE_ONLY_ERROR_HANDLING_API
    DEFINES += ERRORS_PROJ_NAME=$(CONFIG_PROJECT_NAME)
    #only compile following file:
    SRC := errors_api_check.c
endif
VPATH = src

include $(COMMON_CC)
