
INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_DPWM_MIXER)
 


#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  


SRC = dpwm_mixer.cpp

ifdef CONFIG_DSP_IS_SPEED_CRITICAL
    SPEED_CRITICAL_FILES += dpwm_mixer.cpp
endif

VPATH = src


include $(COMMON_CC)
