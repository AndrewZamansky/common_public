
ifeq ($(sort $(CONFIG_I94XXX)),y)
    INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_INTERNAL_PWM)
endif



#INCLUDE_DIR =  

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  


SRC = dpwm_i94xxx.c
SRC += dpwm.c


ifdef CONFIG_INTERNAL_PWM_IS_SPEED_CRITICAL
    SPEED_CRITICAL_FILES += dpwm_i94xxx.c
endif


VPATH = src


include $(COMMON_CC)
