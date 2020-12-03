
ifeq ($(sort $(CONFIG_I94XXX) $(CONFIG_I96XXX_M0)),y)
    INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_INTERNAL_PWM)
endif



#INCLUDE_DIR =  

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  


SRC += dpwm_i9xxxx.c


ifdef CONFIG_INTERNAL_PWM_IS_SPEED_CRITICAL
    SPEED_CRITICAL_FILES += dpwm_i9xxxx.c
endif


VPATH = src

SRC += dpwm.c
ifdef CONFIG_I94XXX
    VPATH += $(I94XXX_SRC_DIR)
else ifdef CONFIG_I96XXX_M0
    VPATH += $(I96XXX_SRC_DIR)
endif


include $(COMMON_CC)
