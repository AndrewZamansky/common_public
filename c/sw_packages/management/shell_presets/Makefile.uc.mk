
INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_SHELL_PRESETS)
INCLUDE_THIS_FOR_H_FILES_PATH := $(CONFIG_INCLUDE_SHELL_PRESETS)
INCLUDE_THIS_FOR_H_FILES_PATH += $(CONFIG_ARM_SEMIHOSTING_CONFIG_ENABLE_RX)


#INCLUDE_DIR = 

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = shell_presets.c


VPATH = src

include $(COMMON_CC)
