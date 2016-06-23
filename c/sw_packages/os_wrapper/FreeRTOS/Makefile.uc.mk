
INCLUDE_THIS_COMPONENT := $(CONFIG_FREE_RTOS)

ifdef CONFIG_FREE_RTOS 	 
    ifeq ("$(wildcard $(EXTERNAL_SOURCE_ROOT_DIR)/FreeRTOS/FreeRTOS/Source/include)","")
        $(info !!!   $(EXTERNAL_SOURCE_ROOT_DIR)/FreeRTOS/FreeRTOS/Source/include don't exists !!!! )
        $(error )
    endif
    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH ,  $(EXTERNAL_SOURCE_ROOT_DIR)/FreeRTOS/FreeRTOS/Source/include )
    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH ,  $(EXTERNAL_SOURCE_ROOT_DIR)/FreeRTOS/FreeRTOS/Demo/Common/include )
    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH ,  $(EXTERNAL_SOURCE_ROOT_DIR)/FreeRTOS/FreeRTOS/Source/portable/GCC/ARM_CM3 )
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
VPATH += $(EXTERNAL_SOURCE_ROOT_DIR)/FreeRTOS/FreeRTOS/Source 
# memory management
SRC += heap_3.c 
VPATH += : $(EXTERNAL_SOURCE_ROOT_DIR)/FreeRTOS/FreeRTOS/Source/portable/MemMang 


SRC += port.c 
ifdef CONFIG_CORTEX_M3
	SRC += freeRtos_cortex_M_port.c
endif
ifdef CONFIG_CORTEX_M4
	SRC += freeRtos_cortex_M_port.c
endif

ifdef CONFIG_GCC	
    ifdef CONFIG_CORTEX_M3
        VPATH += :$(EXTERNAL_SOURCE_ROOT_DIR)/FreeRTOS/FreeRTOS/Source/portable/GCC/ARM_CM3 
    else ifdef CONFIG_CORTEX_M4
        VPATH += :$(EXTERNAL_SOURCE_ROOT_DIR)/FreeRTOS/FreeRTOS/Source/portable/GCC/ARM_CM4F 
    endif
else ifdef CONFIG_GPP	
    ifdef CONFIG_CORTEX_M3
        VPATH += :$(EXTERNAL_SOURCE_ROOT_DIR)/FreeRTOS/FreeRTOS/Source/portable/GCC/ARM_CM3 
    else ifdef CONFIG_CORTEX_M4
        VPATH += :$(EXTERNAL_SOURCE_ROOT_DIR)/FreeRTOS/FreeRTOS/Source/portable/GCC/ARM_CM4F 
    endif
else ifdef CONFIG_ARMCC 
    ifdef CONFIG_CORTEX_M3
        VPATH += :$(EXTERNAL_SOURCE_ROOT_DIR)/FreeRTOS/FreeRTOS/Source/portable/RVDS/ARM_CM3 
    else ifdef CONFIG_CORTEX_M4
        VPATH += :$(EXTERNAL_SOURCE_ROOT_DIR)/FreeRTOS/FreeRTOS/Source/portable/RVDS/ARM_CM4F 
        
    endif     
endif

ifdef CONFIG_RT_OS_IS_SPEED_CRITICAL
    SPEED_CRITICAL_FILES +=os_wrapper_FreeRTOS.c tasks.c  croutine.c event_groups.c list.c queue.c timers.c heap_3.c port.c
endif


 


include $(COMMON_CC)
