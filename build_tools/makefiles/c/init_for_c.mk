#################################################################
### the following section we need to run just one time per build
ifndef INIT_FOR_C_SECTION_THAT_SHOULD_RUN_ONCE

KCONFIG_START_DIR_PATH :=$(COMMON_PUBLIC_DIR)/c

ifeq ($(findstring menuconfig,$(MAKECMDGOALS)),)
    #DONT enter if we are bulding .config file now

    GLOBAL_DEFINES :=
    GLOBAL_INCLUDE_DIR :=
    SUBDIRS :=
    LIBS :=
    LIBRARIES_DIRS :=
    LDS_PREPROCESSOR_FLAGS :=

    include $(PREBUILD_ROUTINES_DIR)/init_project.mk
    
    PROJECT_CONFIG_H_FILE := $(AUTO_GENERATED_FILES_DIR)/project_config.h
    
    SCAN_DIRS_FOR_CONFIG_FILES = $(APP_ROOT_DIR) $(SW_PACKAGES_ROOT_DIR)
    SCAN_DIRS_FOR_CONFIG_FILES += $(DRIVERS_ROOT_DIR) $(COMMON_PRIVATE_DIR)/c
    
    GLOBAL_DEFINES += VERSION_STR="$(FULL_VERSION_STR)"

    GLOBAL_CFLAGS :=
    GLOBAL_ASMFLAGS :=
    GLOBAL_LDFLAGS :=

    ARCHIVE_OUTPUT := $(WORKSPACE_NAME).$(PROJECT_NAME).$(DATE_STR).7z

    #####   global compiler and linker flags :  #####

    ifdef CONFIG_DATA_LOCATION_INTERNAL_SRAM
        RAM_START_ADDR:=$(CONFIG_DT_SOC_RAM_START_ADDR)
        RAM_SIZE:=$(CONFIG_DT_SOC_RAM_SIZE)
    endif
    ifdef CONFIG_DATA_LOCATION_DDR
        RAM_START_ADDR:=$(CONFIG_DDR_START_ADDR)
        RAM_SIZE:=$(CONFIG_DDR_SIZE)
    endif

    ifdef CONFIG_CODE_LOCATION_INTERNAL_SRAM
        ROM_START_ADDR:=$(CONFIG_DT_SOC_RAM_START_ADDR)
        ROM_SIZE:=$(CONFIG_DT_SOC_RAM_SIZE)
    endif
    ifdef CONFIG_CODE_LOCATION_DDR
        ROM_START_ADDR:=$(CONFIG_DDR_START_ADDR)
        ROM_SIZE:=$(CONFIG_DDR_SIZE)
    endif
    ifdef CONFIG_CODE_LOCATION_INTERNAL_ROM
        ROM_START_ADDR:=$(CONFIG_INTERNAL_ROM_START_ADDR)
        ROM_SIZE:=$(CONFIG_INTERNAL_ROM_SIZE)
    endif
    ifdef CONFIG_CODE_LOCATION_INTERNAL_FLASH
        ROM_START_ADDR:=$(CONFIG_DT_SOC_FLASH_START_ADDR)
        ROM_SIZE:=$(CONFIG_DT_SOC_FLASH_SIZE)
    endif
    ifdef CONFIG_CODE_LOCATION_EXTERNAL_FLASH
        ROM_START_ADDR:=$(CONFIG_EXTERNAL_FLASH_START_ADDR)
        ROM_SIZE:=$(CONFIG_EXTERNAL_FLASH_SIZE)
    endif

    GLOBAL_DEFINES += RAM_START_ADDR=$(RAM_START_ADDR)


    COMPILERS_DIR :=$(MAKEFILES_ROOT_DIR)/c/compilers
    ifdef CONFIG_ARM
        ifdef CONFIG_GCC
            include $(COMPILERS_DIR)/gcc/gcc_arm_init.mk
        else ifdef CONFIG_GPP
            include $(COMPILERS_DIR)/gcc/gcc_arm_init.mk
        else ifdef CONFIG_ARMCC
            include $(COMPILERS_DIR)/armcc/armcc_init.mk
        endif
    else ifdef CONFIG_PIC32
        include $(COMPILERS_DIR)/gcc/gcc_pic32_init.mk
    else ifdef CONFIG_AVR
        include $(COMPILERS_DIR)/gcc/gcc_avr_init.mk
    else ifdef CONFIG_TENSILICA
        include $(COMPILERS_DIR)/gcc/gcc_tensilica_init.mk
    else ifdef CONFIG_STM8
        include $(COMPILERS_DIR)/cxstm8/cxstm8_init.mk
    else ifdef CONFIG_HEXAGON
        include $(COMPILERS_DIR)/clang/clang_hexagon_init.mk
    else ifdef CONFIG_ANDROID_NDK
        include $(COMPILERS_DIR)/gcc/gcc_android_init.mk
    else ifdef CONFIG_HOST
        ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
            ifdef CONFIG_MIN_GW_GCC
                include $(COMPILERS_DIR)/gcc/gcc_host_init.mk
            else ifdef CONFIG_MICROSOFT_COMPILER
                include $(COMPILERS_DIR)/microsoft_compiler/msvc_host_init.mk
            endif
        else
            include $(COMPILERS_DIR)/gcc/gcc_host_init.mk
        endif
    else
        $(error ---- unknown compiler ----)
    endif


    #########################################################
    #####   components configuration includes folder :  #####  
    GLOBAL_INCLUDE_DIR += $(APP_ROOT_DIR)/project_config_includes
    GLOBAL_INCLUDE_DIR += $(APP_ROOT_DIR)/z_auto_generated_files
endif


INIT_FOR_C_SECTION_THAT_SHOULD_RUN_ONCE = dummy_value

export

endif####
### end of section that run just one time per build
#####################################################
