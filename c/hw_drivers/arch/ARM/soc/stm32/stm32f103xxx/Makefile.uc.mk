
ifeq ($(sort $(CONFIG_STM32F10X)),y)

    STM32F103XXX_BSP_VERSION :=$(patsubst "%",%,$(CONFIG_STM32F10X_BSP_VERSION))
    _BSP_DIR_NAME =STM32F10x_StdPeriph_Lib_V$(STM32F103XXX_BSP_VERSION)
    BSP_STM32F10X_PATH :=$(EXTERNAL_SOURCE_ROOT_DIR)/ST/$(_BSP_DIR_NAME)
    ifeq ("$(wildcard $(BSP_STM32F10X_PATH))","")
        $(info !--- STM32F10X BSP path $(BSP_STM32F10X_PATH) dont exists )
        $(info !--- download STM32F10X BSP version $(STM32F103XXX_BSP_VERSION),)
        $(info !--- unpack it to $(BSP_STM32F10X_PATH)  )
        $(info !--- make sure Libraries and Project folders is located in $(BSP_STM32F10X_PATH)/  after unpacking   )
        $(error )
    endif
    
    ifeq ("","$(filter $(BSP_STM32F10X_PATH),$(EXTERNAL_SRC_DIRS))")
        EXTERNAL_SRC_DIRS := $(EXTERNAL_SRC_DIRS) $(BSP_STM32F10X_PATH)
    endif

    STM32F10X_LIB_DIR :=$(BSP_STM32F10X_PATH)/Libraries

    STM32F10X_DRIVERS_DIR :=$(STM32F10X_LIB_DIR)/STM32F10x_StdPeriph_Driver
    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(STM32F10X_DRIVERS_DIR)/inc)

    STM32F10X_DEV_SUPPORT_DIR :=$(STM32F10X_LIB_DIR)/CMSIS/CM3/DeviceSupport
    STM32F10X_H_FILE_DIR :=$(STM32F10X_DEV_SUPPORT_DIR)/ST/STM32F10x
    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(STM32F10X_H_FILE_DIR))

	STM32_MEDIUM_DENSITY := $(CONFIG_STM32F103x8_64K_FLASH)
	STM32_MEDIUM_DENSITY += $(CONFIG_STM32F103xB_128K_FLASH)
    ifeq ($(sort $(STM32_MEDIUM_DENSITY)),y)
        DUMMY := $(call ADD_TO_GLOBAL_DEFINES ,STM32F10X_MD )
    endif

	STM32_HIGH_DENSITY := $(CONFIG_STM32F103xC_256K_FLASH)
	STM32_HIGH_DENSITY += $(CONFIG_STM32F103xD_384K_FLASH)
	STM32_HIGH_DENSITY += $(CONFIG_STM32F103xE_512K_FLASH)
    ifeq ($(sort $(STM32_HIGH_DENSITY)),y)
        DUMMY := $(call ADD_TO_GLOBAL_DEFINES ,STM32F10X_HD )
    endif

    #DUMMY := $(call ADD_TO_GLOBAL_DEFINES ,USE_STM3210B_EVAL )
    DUMMY := $(call ADD_TO_GLOBAL_DEFINES ,USE_STDPERIPH_DRIVER )
    STM32F10X_SRC_DIR := $(STM32F10X_DRIVERS_DIR)/src
    INCLUDE_THIS_COMPONENT := y

endif

SRC = board_init.c

VPATH = src

include $(COMMON_CC)
