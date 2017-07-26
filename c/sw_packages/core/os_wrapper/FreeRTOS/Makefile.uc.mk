
INCLUDE_THIS_COMPONENT := $(CONFIG_FREE_RTOS)

ifdef CONFIG_FREE_RTOS 	 

    FREE_RTOS_VERSION :=$(patsubst "%",%,$(CONFIG_FREE_RTOS_VERSION))
    FREE_RTOS_PATH :=$(EXTERNAL_SOURCE_ROOT_DIR)/FreeRTOSv$(FREE_RTOS_VERSION)
    ifeq ("$(wildcard $(FREE_RTOS_PATH))","")
        $(info  )
        $(info !--- freeRTOS path $(FREE_RTOS_PATH) dont exists )
        $(info !--- download freeRTOS version $(FREE_RTOS_VERSION) and unpack it to $(FREE_RTOS_PATH)  )
        $(info !--- make sure that file Quick_Start_Guide is located in $(FREE_RTOS_PATH)/  after unpacking   )
        $(info  )
        $(error )
    endif
    
    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH ,  $(FREE_RTOS_PATH)/FreeRTOS/Source/include )
    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH ,  $(FREE_RTOS_PATH)/FreeRTOS/Demo/Common/include )
    ifdef CONFIG_CORTEX_M4
        DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH ,  $(FREE_RTOS_PATH)/FreeRTOS/Source/portable/GCC/ARM_CM3 )
    else ifdef CONFIG_XTENSA_XCC
        DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH ,  $(FREE_RTOS_PATH)/FreeRTOS/Source/portable/XCC/Xtensa )
    endif
    #used in external sources like wolfssl, ...
    DUMMY := $(call ADD_TO_GLOBAL_DEFINES , FREERTOS)
endif  


#INCLUDE_DIR = 

#DEFINES = 

ifdef CONFIG_GPP
    CFLAGS = -fpermissive
endif



#ASMFLAGS =  



SRC = os_wrapper_FreeRTOS.c 
VPATH = src

# main freeRTOS files
SRC += tasks.c 
SRC += croutine.c
SRC += event_groups.c
SRC += list.c
SRC += queue.c
SRC += timers.c
VPATH += $(FREE_RTOS_PATH)/FreeRTOS/Source 
# memory management
SRC += heap_3.c
#SRC += heap_4.c
VPATH += : $(FREE_RTOS_PATH)/FreeRTOS/Source/portable/MemMang 


SRC += port.c 
ifdef CONFIG_CORTEX_M3
	SRC += freeRtos_cortex_M_port.c
else ifdef CONFIG_CORTEX_M4
	SRC += freeRtos_cortex_M_port.c
else ifdef 	CONFIG_XTENSA_XCC
	SRC += portasm.S
	SRC += xtensa_context.S
	SRC += portclib.c
	SRC += xtensa_overlay_os_hook.c
	SRC += freeRtos_xtensa_port.c
endif

ifdef CONFIG_GCC	
    ifdef CONFIG_CORTEX_M3
        VPATH += :$(FREE_RTOS_PATH)/FreeRTOS/Source/portable/GCC/ARM_CM3
    else ifdef CONFIG_CORTEX_M4
        ifdef CONFIG_INCLUDE_CORTEX_M_FPU
            VPATH += :$(FREE_RTOS_PATH)/FreeRTOS/Source/portable/GCC/ARM_CM4F
        else
            VPATH += :$(FREE_RTOS_PATH)/FreeRTOS/Source/portable/GCC/ARM_CM3
        endif 
    endif
else ifdef CONFIG_GPP	
    ifdef CONFIG_CORTEX_M3
        VPATH += :$(FREE_RTOS_PATH)/FreeRTOS/Source/portable/GCC/ARM_CM3
    else ifdef CONFIG_CORTEX_M4
        ifdef CONFIG_INCLUDE_CORTEX_M_FPU
            VPATH += :$(FREE_RTOS_PATH)/FreeRTOS/Source/portable/GCC/ARM_CM4F
        else
            VPATH += :$(FREE_RTOS_PATH)/FreeRTOS/Source/portable/GCC/ARM_CM3
        endif 
    endif
else ifdef CONFIG_ARMCC 
    ifdef CONFIG_CORTEX_M3
        VPATH += :$(FREE_RTOS_PATH)/FreeRTOS/Source/portable/RVDS/ARM_CM3
    else ifdef CONFIG_CORTEX_M4
        ifdef CONFIG_INCLUDE_CORTEX_M_FPU
            VPATH += :$(FREE_RTOS_PATH)/FreeRTOS/Source/portable/RVDS/ARM_CM4F
        else
            VPATH += :$(FREE_RTOS_PATH)/FreeRTOS/Source/portable/RVDS/ARM_CM3
        endif         
    endif
else ifdef CONFIG_XTENSA_XCC 
    VPATH += :$(FREE_RTOS_PATH)/FreeRTOS/Source/portable/XCC/Xtensa
endif


ifdef CONFIG_RT_OS_IS_SPEED_CRITICAL
    SPEED_CRITICAL_FILES +=os_wrapper_FreeRTOS.c tasks.c  croutine.c port.c
    SPEED_CRITICAL_FILES +=event_groups.c list.c queue.c timers.c heap_3.c
endif


 

include $(COMMON_CC)
