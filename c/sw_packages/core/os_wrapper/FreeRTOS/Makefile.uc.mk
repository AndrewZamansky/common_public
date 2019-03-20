
ifeq ($(sort $(CONFIG_FREE_RTOS)),y)
	INCLUDE_THIS_COMPONENT := y

    FREE_RTOS_VERSION :=$(patsubst "%",%,$(CONFIG_FREE_RTOS_VERSION))
    FREE_RTOS_PATH :=$(EXTERNAL_SOURCE_ROOT_DIR)/FreeRTOSv$(FREE_RTOS_VERSION)
    ifeq ("$(wildcard $(FREE_RTOS_PATH))","")
        $(info  )
        $(info !--- freeRTOS path $(FREE_RTOS_PATH) dont exists)
        $(info !--- download freeRTOS version $(FREE_RTOS_VERSION) and unpack it to $(FREE_RTOS_PATH))
        $(info !--- make sure that file Quick_Start_Guide is located in $(FREE_RTOS_PATH)/  after unpacking)
        $(info  )
        $(error )
    endif

    ifeq ("","$(filter $(FREE_RTOS_PATH),$(EXTERNAL_SRC_DIRS))")
        EXTERNAL_SRC_DIRS := $(EXTERNAL_SRC_DIRS) $(FREE_RTOS_PATH)
    endif

    MAIN_FREE_RTOS_INCLUDE_PATH :=$(FREE_RTOS_PATH)/FreeRTOS/Source/include
    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(MAIN_FREE_RTOS_INCLUDE_PATH))

    FREE_RTOS_PORT_PATH :=$(FREE_RTOS_PATH)/FreeRTOS/Source/portable
    ifdef CONFIG_CORTEX_M3
        FREE_RTOS_PORT_INCLUDE_PATH :=$(FREE_RTOS_PORT_PATH)/GCC/ARM_CM3
    else ifdef CONFIG_CORTEX_M4
        ifeq ($(sort $(CONFIG_INCLUDE_CORTEX_M_FPU)),y)
            FREE_RTOS_PORT_INCLUDE_PATH :=$(FREE_RTOS_PORT_PATH)/GCC/ARM_CM3
        else
            FREE_RTOS_PORT_INCLUDE_PATH :=$(FREE_RTOS_PORT_PATH)/GCC/ARM_CM4F
        endif
    else ifdef CONFIG_XTENSA_XCC
        FREE_RTOS_PORT_INCLUDE_PATH :=$(FREE_RTOS_PORT_PATH)/XCC/Xtensa
    endif
    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(FREE_RTOS_PORT_INCLUDE_PATH))

    #used in external sources like wolfssl, ...
    DUMMY := $(call ADD_TO_GLOBAL_DEFINES , FREERTOS)
endif  


#INCLUDE_DIR = 

#DEFINES = 

ifdef CONFIG_GPP
    CFLAGS = -fpermissive
endif



#ASMFLAGS =  



SRC = src/os_wrapper_FreeRTOS.c 

# main freeRTOS files
SRC += FreeRTOS/Source/tasks.c 
SRC += FreeRTOS/Source/croutine.c
SRC += FreeRTOS/Source/event_groups.c
SRC += FreeRTOS/Source/list.c
SRC += FreeRTOS/Source/queue.c
SRC += FreeRTOS/Source/timers.c
# memory management
SRC += FreeRTOS/Source/portable/MemMang/heap_3.c
#SRC += FreeRTOS/Source/portable/MemMang/heap_4.c


ifeq ($(sort $(CONFIG_CORTEX_M3)),y)
	SRC += src/freeRtos_cortex_M_port.c
    ifeq ($(sort $(CONFIG_GCC) $(CONFIG_GPP)),y)
        SRC += FreeRTOS/Source/portable/GCC/ARM_CM3/port.c
    else ifdef CONFIG_ARMCC 
        SRC += FreeRTOS/Source/portable/RVDS/ARM_CM3/port.c
    endif
else ifeq ($(sort $(CONFIG_CORTEX_M4)),y)
	SRC += src/freeRtos_cortex_M_port.c
    ifeq ($(sort $(CONFIG_GCC) $(CONFIG_GPP)),y)
        ifeq ($(sort $(CONFIG_INCLUDE_CORTEX_M_FPU)),y)
            SRC += FreeRTOS/Source/portable/GCC/ARM_CM4F/port.c
        else
            SRC += FreeRTOS/Source/portable/GCC/ARM_CM3/port.c
        endif
    else ifdef CONFIG_ARMCC 
        ifeq ($(sort $(CONFIG_INCLUDE_CORTEX_M_FPU)),y)
            SRC += FreeRTOS/Source/portable/RVDS/ARM_CM4F/port.c
        else
            SRC += FreeRTOS/Source/portable/RVDS/ARM_CM3/port.c
        endif
    endif
else ifeq ($(sort $(CONFIG_XTENSA_XCC)),y)
	SRC += FreeRTOS/Source/portable/XCC/Xtensa/portasm.S
	SRC += FreeRTOS/Source/portable/XCC/Xtensa/xtensa_context.S
	SRC += FreeRTOS/Source/portable/XCC/Xtensa/portclib.c
	SRC += FreeRTOS/Source/portable/XCC/Xtensa/xtensa_overlay_os_hook.c
	SRC += src/freeRtos_xtensa_port.c
endif

VPATH += | $(FREE_RTOS_PATH)

ifdef CONFIG_RT_OS_IS_SPEED_CRITICAL
    SPEED_CRITICAL_FILES +=$(SRC)
endif



include $(COMMON_CC)
