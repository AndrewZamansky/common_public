
ifeq ($(findstring freeRtos,$(CONFIG_INCLUDE_OS)),freeRtos) 	 
	GLOBAL_INCLUDE_DIR := $(GLOBAL_INCLUDE_DIR) $(EXTERNAL_SOURCE_ROOT_DIR)/FreeRTOS/FreeRTOS/Source/include
	GLOBAL_INCLUDE_DIR := $(GLOBAL_INCLUDE_DIR) $(EXTERNAL_SOURCE_ROOT_DIR)/FreeRTOS/FreeRTOS/Demo/Common/include
	GLOBAL_INCLUDE_DIR := $(GLOBAL_INCLUDE_DIR) $(EXTERNAL_SOURCE_ROOT_DIR)/FreeRTOS/FreeRTOS/Source/portable/GCC/ARM_CM3
	GLOBAL_DEFINES := $(GLOBAL_DEFINES) PREDEFINED_OS_SWI_HANDLER=vPortSVCHandler
	GLOBAL_DEFINES := $(GLOBAL_DEFINES) OS_FREE_RTOS=1
	_NO_OS_FOUND = NO
	INCLUDE_THIS_COMPONENT := YES   # must be here !!
endif  


#INCLUDE_DIR = 

#DEFINES = 

#CFLAGS = 

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
ifeq ($(findstring GCC,$(CONFIG_USE_COMPILER)),GCC) 	
    ifeq ($(findstring cortex-m3,$(CONFIG_CPU_TYPE)),cortex-m3) 	
        VPATH += :$(EXTERNAL_SOURCE_ROOT_DIR)/FreeRTOS/FreeRTOS/Source/portable/GCC/ARM_CM3 
    else
        VPATH += :$(EXTERNAL_SOURCE_ROOT_DIR)/FreeRTOS/FreeRTOS/Source/portable/GCC/ARM_CM4F 
    endif
else ifeq ($(findstring ARMCC,$(CONFIG_USE_COMPILER)),ARMCC) 
    ifeq ($(findstring cortex-m3,$(CONFIG_CPU_TYPE)),cortex-m3) 	
        VPATH += :$(EXTERNAL_SOURCE_ROOT_DIR)/FreeRTOS/FreeRTOS/Source/portable/RVDS/ARM_CM3 
    else
        VPATH += :$(EXTERNAL_SOURCE_ROOT_DIR)/FreeRTOS/FreeRTOS/Source/portable/RVDS/ARM_CM4F 
    endif     
endif

ifeq ($(findstring AS_SPEED_CRITICAL,$(CONFIG_INCLUDE_OS)),AS_SPEED_CRITICAL) 
    SPEED_CRITICAL_FILES +=os_wrapper_FreeRTOS.c tasks.c  croutine.c event_groups.c list.c queue.c timers.c heap_3.c port.c
endif


# cortex-M3 porting


include $(COMMON_CC)
