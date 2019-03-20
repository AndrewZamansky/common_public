
INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_PCM_SPLITTER)
 

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  

SRC = pcm_splitter.cpp

ifdef CONFIG_DSP_IS_SPEED_CRITICAL
    SPEED_CRITICAL_FILES += pcm_splitter.cpp
endif


VPATH = src


include $(COMMON_CC)
