
INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_DPWM_MIXER)
 


#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  


SRC = dpwm_mixer.c

ifdef CONFIG_DPWM_MIXER_IS_SPEED_CRITICAL
    SPEED_CRITICAL_FILES += dpwm_mixer.c
endif

VPATH = src


include $(COMMON_CC)