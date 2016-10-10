INCLUDE_THIS_COMPONENT := $(CONFIG_OSA)
ifdef CONFIG_OSA

    OSA_RTOS_PATH :=$(EXTERNAL_SOURCE_ROOT_DIR)/osa_v110306
    ifeq ("$(wildcard $(OSA_RTOS_PATH))","")
        $(info  )
        $(info !--- OSA path $(OSA_RTOS_PATH) dont exists )
        $(info !--- download OSA version 110306 and unpack it to $(OSA_RTOS_PATH)  )
        $(info !--- make sure that kernel and ports folders are located in $(OSA_RTOS_PATH)/  after unpacking   )
        $(info  )
        $(error )
    endif
    
    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH ,  $(OSA_RTOS_PATH) )
    
    ifdef CONFIG_STM8
        DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH ,  $(OSA_RTOS_PATH)/ports/stm8 )
    endif
    ifdef CONFIG_AVR
        DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH ,  $(OSA_RTOS_PATH)/ports/avr )
    endif
    
endif


#INCLUDE_DIR = 

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = os_wrapper_osa.c
VPATH = src

# main OSA files
SRC += osa.c
VPATH += |$(OSA_RTOS_PATH)


# memory management

ifdef CONFIG_STM8

     SRC += stm8s_tim1.c 
     VPATH += |$(STM8S_SRC_DIR)

endif

include $(COMMON_CC)
