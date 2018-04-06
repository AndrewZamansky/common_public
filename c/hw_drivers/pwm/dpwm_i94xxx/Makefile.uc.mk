
ifeq ($(sort $(CONFIG_I94XXX)),y)
    INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_INTERNAL_PWM)
endif



#INCLUDE_DIR =  

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  


SRC += dpwm_i94xxx.c


ifdef CONFIG_INTERNAL_PWM_IS_SPEED_CRITICAL
    SPEED_CRITICAL_FILES += dpwm_i94xxx.c
endif


VPATH = src

SRC += dpwm.c
VPATH += | $(I94XXX_SRC_DIR)


include $(COMMON_CC)
