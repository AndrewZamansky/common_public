
ifeq ($(sort $(CONFIG_OSA)),y)
	INCLUDE_THIS_COMPONENT := y

    OSA_RTOS_PATH :=$(EXTERNAL_SOURCE_ROOT_DIR)/osa_v110306
    ifeq ("$(wildcard $(OSA_RTOS_PATH))","")
        $(info  )
        $(info !--- OSA path $(OSA_RTOS_PATH) does not exist)
        $(info !--- download OSA version 110306 and)
        $(info !--- unpack it to $(OSA_RTOS_PATH))
        $(info !--- make sure that kernel and ports folders are)
        $(info !--- located in $(OSA_RTOS_PATH)/  after unpacking)
        $(info  )
        $(error )
    endif
    
    ifeq ("","$(filter $(OSA_RTOS_PATH),$(EXTERNAL_SRC_DIRS))")
        EXTERNAL_SRC_DIRS := $(EXTERNAL_SRC_DIRS) $(OSA_RTOS_PATH)
    endif

    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH, $(OSA_RTOS_PATH))
    
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




include $(COMMON_CC)
