
ifdef CONFIG_ARM
    INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_SEMIHOSTING)
endif

#INCLUDE_DIR =  

#DEFINES :=

#CFLAGS = 

#ASMFLAGS =  

SRC = semihosting.c


VPATH = src


include $(COMMON_CC)
