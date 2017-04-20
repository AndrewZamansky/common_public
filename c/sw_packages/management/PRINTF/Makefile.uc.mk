
INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_PRINTF)
#INCLUDE_THIS_FOR_H_FILES_PATH := y # always include path to PRINTF_api.h

DEFINES = 

#CFLAGS = 

#ASMFLAGS =  

ifdef CONFIG_MICROSOFT_COMPILER
	DEFINES += _CRT_SECURE_NO_WARNINGS # to disable deprecation in windows compiler
endif


SRC = PRINTF.c 


VPATH = src

include $(COMMON_CC)
