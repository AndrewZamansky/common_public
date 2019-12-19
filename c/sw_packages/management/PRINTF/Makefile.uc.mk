
INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_PRINTF)
INCLUDE_THIS_FOR_H_FILES_PATH := y # always include path to PRINTF_api.h

DEFINES = 

#CFLAGS = 

#ASMFLAGS =  


ifeq ($(CONFIG_HOST),y)
    DEFINES += COMPILING_FOR_HOST
    ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
        ifdef CONFIG_MICROSOFT_COMPILER
            # to disable deprecation in windows compiler
            DEFINES += _CRT_SECURE_NO_WARNINGS
        endif
        DEFINES += COMPILING_FOR_WINDOWS_HOST
    else
        DEFINES += COMPILING_FOR_LINUX_HOST
    endif
endif
SRC = PRINTF.c 


VPATH = src

include $(COMMON_CC)
