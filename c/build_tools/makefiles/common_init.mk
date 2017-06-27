

#################################################################
### the following section we need to run just one time per build
ifndef COMMON_INIT_SECTION_THAT_SHOULD_RUN_ONCE

MAKEFILES_ROOT_DIR :=$(abspath $(MAKEFILES_ROOT_DIR))

include $(MAKEFILES_ROOT_DIR)/common_init_environment.mk
include $(MAKEFILES_ROOT_DIR)/common.mk


GLOBAL_PROJECT_SPECIFIC_CFLAGS :=
GLOBAL_PROJECT_SPECIFIC_ASMLAGS :=
GLOBAL_PROJECT_SPECIFIC_LDFLAGS :=
GLOBAL_DEFINES :=
GLOBAL_INCLUDE_DIR :=
SUBDIRS :=
LIBS :=
LIBRARIES_DIRS :=
LDS_PREPROCESSOR_FLAGS :=

#find "common_public/c" directory path :
ifneq ($(wildcard $(APP_ROOT_DIR)/../common_public/c),)
    COMMON_ROOT_DIR    :=    $(APP_ROOT_DIR)/../common_public/c
else ifneq ($(wildcard $(APP_ROOT_DIR)/../../common_public/c),)
    COMMON_ROOT_DIR    :=    $(APP_ROOT_DIR)/../../common_public/c
else ifneq ($(wildcard $(APP_ROOT_DIR)/../../../common_public/c),)
    COMMON_ROOT_DIR    :=    $(APP_ROOT_DIR)/../../../common_public/c
else ifneq ($(wildcard $(APP_ROOT_DIR)/../../../../common_public/c),)
    COMMON_ROOT_DIR    :=    $(APP_ROOT_DIR)/../../../../common_public/c
else
    $(error !---- COMMON C \(common_public/c\) DIR NOT FOUND ----)
endif
COMMON_ROOT_DIR :=$(abspath $(COMMON_ROOT_DIR))


WORKSPACE_ROOT_DIR := $(patsubst $(APP_ROOT_DIR)/%,%,$(realpath $(COMMON_ROOT_DIR)/../..))
WORKSPACE_NAME := $(notdir $(WORKSPACE_ROOT_DIR))

EXTERNAL_SOURCE_ROOT_DIR := $(patsubst %/,%,$(dir $(WORKSPACE_ROOT_DIR)))/external_source
ifeq ("$(wildcard $(EXTERNAL_SOURCE_ROOT_DIR))","")
    $(info !--- directory $(EXTERNAL_SOURCE_ROOT_DIR) dont exists . create it .)
    $(error )
endif


include $(WORKSPACE_ROOT_DIR)/workspace_config.mk

MAKEFILES_INC_FUNC_DIR :=$(MAKEFILES_ROOT_DIR)/_common_include_functions
DRIVERS_ROOT_DIR := $(COMMON_ROOT_DIR)/hw_drivers
SW_PACKAGES_ROOT_DIR := $(COMMON_ROOT_DIR)/sw_packages

AUTO_GENERATED_FILES_DIR := $(APP_ROOT_DIR)/z_auto_generated_files
OUT_DIR_HISTORY    :=    $(APP_ROOT_DIR)/zOUT_history

MKDIR=mkdir    

COMMON_DIR = $(WORKSPACE_ROOT_DIR)/common_public
COMMON_PRIVATE_DIR = $(WORKSPACE_ROOT_DIR)/common_private

EMPTY:=
SPACE:= $(EMPTY) $(EMPTY)

####### test for existence of make and put its directory name in MAKE_DIR #####
SEARCHED_TOOL:=make
SEARCHED_DIR_VARIABLE:=MAKE_DIR
MANUALLY_DEFINED_DIR_VARIABLE:=REDEFINE_MAKE_PROGRAM_DIR
ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
    TEST_FILE_IN_SEARCHED_DIR:=bin\make.exe
else ifeq ($(findstring LINUX,$(COMPILER_HOST_OS)),LINUX)
    TEST_FILE_IN_SEARCHED_DIR:=bin/make
endif
include $(MAKEFILES_ROOT_DIR)/_common_include_functions/tool_existence_check.mk
####### end of tool existence test #####

MAKE :="$(MAKE_DIR)/bin/make"

ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)

    #replace backslash for slash
    APP_ROOT_DIR_WINDOWS := $(subst /,\,$(APP_ROOT_DIR))
    AUTO_GENERATED_FILES_DIR := $(subst /,\,$(AUTO_GENERATED_FILES_DIR))
    OUT_DIR_HISTORY := $(subst /,\,$(OUT_DIR_HISTORY))
    COMMON_DIR := $(subst /,\,$(COMMON_DIR))
    COMMON_PRIVATE_DIR := $(subst /,\,$(COMMON_PRIVATE_DIR))

#    CRC32CALC    =    $(TOOLS_ROOT_DIR)\crc32\crc32.exe



    CONFIG_SEMIHOSTING_UPLOADING_DIR :=c:\Temp
    ifeq ($(wildcard $(CONFIG_SEMIHOSTING_UPLOADING_DIR)),)         #if $(CONFIG_SEMIHOSTING_UPLOADING_DIR) dont exists then $(wildcard $(CONFIG_SEMIHOSTING_UPLOADING_DIR)) will produce empty string 
       DUMMY:=$(shell $(MKDIR)  $(CONFIG_SEMIHOSTING_UPLOADING_DIR)) # create   $(CONFIG_SEMIHOSTING_UPLOADING_DIR)
    endif

    COMMON_PARTITION := $(firstword $(subst :, ,$(COMMON_DIR))):
    APP_PARTITION := $(firstword $(subst :, ,$(APP_ROOT_DIR))):
    RM        :=rmdir /S /Q
    CP        :=copy /Y

    SHELL_OUTPUT :=$(shell WMIC Path Win32_LocalTime Get Year /value)
    YEAR := $(word 2,$(subst =,$(SPACE),$(strip $(SHELL_OUTPUT))))
    SHELL_OUTPUT :=$(shell WMIC Path Win32_LocalTime Get Month /value)
    MONTH := $(word 2,$(subst =,$(SPACE),$(strip $(SHELL_OUTPUT))))
    ifeq (1,$(MONTH))
        MONTH :=01
    else
        ifneq ($(patsubst 1%,X,$(MONTH)),X)
            MONTH :=0$(MONTH)
        endif
    endif

    SHELL_OUTPUT :=$(shell WMIC Path Win32_LocalTime Get DAY /value)
    DAY := $(word 2,$(subst =,$(SPACE),$(strip $(SHELL_OUTPUT))))
    ifeq (1,$(DAY))
        DAY :=01
    else ifeq (2,$(DAY))
        DAY :=02
    else ifeq (3,$(DAY))
        DAY :=03
    else 
        ifneq ($(patsubst 1%,X,$(DAY)),X)
            ifneq ($(patsubst 2%,X,$(DAY)),X)
                ifneq ($(patsubst 3%,X,$(DAY)),X)
                    DAY :=0$(DAY)
                endif
            endif
        endif
    endif

    DATE    :=$(YEAR)-$(MONTH)-$(DAY)

    SHELL_OUTPUT :=$(shell WMIC Path Win32_LocalTime Get Hour /value)
    HOUR := $(word 2,$(subst =,$(SPACE),$(strip $(SHELL_OUTPUT))))
    SHELL_OUTPUT :=$(shell WMIC Path Win32_LocalTime Get Minute /value)
    Minute := $(word 2,$(subst =,$(SPACE),$(strip $(SHELL_OUTPUT))))
    TIME    :=$(HOUR):$(Minute)
    SHELL_CMD_DELIMITER = &
    CD :=  $(COMMON_PARTITION) & cd

else ifeq ($(findstring LINUX,$(COMPILER_HOST_OS)),LINUX)

    CONFIG_SEMIHOSTING_UPLOADING_DIR :=/tmp

    CRC32CALC    =    cksum 

    MAKE     :=     make
    RM        :=rm -rf
    CP        :=cp -f
    DATE    :=$(shell date "+%Y-%m-%d") 
    TIME    :=$(shell date "+%H:%M") 
    SHELL_CMD_DELIMITER = ;
    CD :=  cd

endif

ifeq ("$(wildcard $(COMMON_PRIVATE_DIR))","")#if common_private directory dont exists then create a dummy one
    DUMMY:=$(shell $(MKDIR)  $(COMMON_PRIVATE_DIR)) # create   $(COMMON_PRIVATE_DIR)
    DUMMY:=$(shell config PRIVATE_DUMMY>$(COMMON_PRIVATE_DIR)/Kconfig) # create   $(COMMON_PRIVATE_DIR)
endif

ifeq ($(findstring menuconfig,$(MAKECMDGOALS)),)
    #DONT enter if we are bulding .config file now

    ifeq ("$(wildcard .config)","")
        $(info !--- .config file dont exists . run 'make menuconfig')
        $(error )
    endif

    include .config

    CONFIG_PROJECT_NAME :=$(strip $(CONFIG_PROJECT_NAME))
    CONFIG_PROJECT_NAME :=$(patsubst "%",%,$(CONFIG_PROJECT_NAME))
    CONFIG_PROJECT_NAME_SUFFIX :=$(strip $(CONFIG_PROJECT_NAME_SUFFIX))
    CONFIG_PROJECT_NAME_SUFFIX :=$(patsubst "%",%,$(CONFIG_PROJECT_NAME_SUFFIX))

    PROJECT_NAME :=$(CONFIG_PROJECT_NAME)
    ifeq (,$(CONFIG_PROJECT_NAME_SUFFIX))
        FULL_PROJECT_NAME :=$(CONFIG_PROJECT_NAME)
    else
        FULL_PROJECT_NAME :=$(CONFIG_PROJECT_NAME)_$(CONFIG_PROJECT_NAME_SUFFIX)
    endif

    ifeq ($(PROJECT_NAME),)      # if $(PROJECT_NAME) is empty
        $(info error : project have to be named set CONFIG_PROJECT_NAME in .config or using menuconfig)
        $(error )
    endif
    $(info ---- project directory : $(APP_ROOT_DIR) ---- )
    $(info ---- project name as declared in .config : $(PROJECT_NAME) ---- )

    $(info ---- running unique project name test ---- )
    PASS_VARIABLES := MAKEFILES_ROOT_DIR=$(MAKEFILES_ROOT_DIR)
    PASS_VARIABLES += WORKSPACE_ROOT_DIR=$(WORKSPACE_ROOT_DIR)
    PASS_VARIABLES += APP_ROOT_DIR=$(APP_ROOT_DIR)
    SHELL_OUT := $(shell $(MAKE) -f $(MAKEFILES_ROOT_DIR)/prebuild_check_unique_project_name.mk $(PASS_VARIABLES)  2>&1)

    ifneq ($(findstring name is not unique,$(SHELL_OUT)),)#enter if name is not unique
        $(info )
        $(info !--- current PROJECT_NAME=$(PROJECT_NAME) , this name is already found as project name in other project)
        $(info !--- all project names , assigned to PROJECT_NAME in .config files should be unique)
        $(error )
    endif
    $(info ---- unique project name test passed ---- )

    include $(MAKEFILES_ROOT_DIR)/git_prebuild_routines.mk

endif

ifneq ($(REDEFINE_OUTPUT_DIR),)
    OBJ_DIR :=$(REDEFINE_OUTPUT_DIR)/$(FULL_PROJECT_NAME)/zOBJ
    OUT_DIR :=$(REDEFINE_OUTPUT_DIR)/$(FULL_PROJECT_NAME)/zOUT
else
    OBJ_DIR    :=$(APP_ROOT_DIR)/zOBJ
    OUT_DIR    :=$(APP_ROOT_DIR)/zOUT    
endif

ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
    OBJ_DIR :=$(subst /,\,$(OBJ_DIR))
    OUT_DIR :=$(subst /,\,$(OUT_DIR))
endif

COMPONENTS_MK := $(AUTO_GENERATED_FILES_DIR)/include_components.mk
PROJECT_CONFIG_H_FILE := $(AUTO_GENERATED_FILES_DIR)/project_config.h
GENERATED_KCONFIG := $(AUTO_GENERATED_FILES_DIR)/Kconfig

OUTPUT_APP_NAME := out
ifdef CONFIG_OUTPUT_NAME
    OUTPUT_APP_NAME := $(CONFIG_OUTPUT_NAME)
endif

OUTPUT_CRC32 :=  $(OUT_DIR)/$(OUTPUT_APP_NAME).crc32



### calculate date
DATE_STR := $(subst -,$(SPACE),$(strip $(DATE)))#replace '-' with ' '
DATE_STR := $(subst /,$(SPACE),$(DATE_STR))#replace '/' with ' '

DATE_YEAR_STR :=$(word 1,$(DATE_STR))
DATE_STR := $(patsubst 20%,%,$(DATE_YEAR_STR)) $(wordlist 2,3,$(DATE_STR))#convert year 20XX to XX and add rest of date
DATE_STR := $(subst $(SPACE),.,$(DATE_STR))#replace ' ' with '.'

### calculating version number 
TIME_STR := $(subst :,.,$(strip $(TIME)))#replace ':' with '.'
CURR_GIT_VERSION :=$(patsubst $(PROJECT_NAME)%,%,$(CURR_GIT_BRANCH))

CURR_GIT_VERSION :=$(patsubst _%,%,$(CURR_GIT_VERSION))
ifeq ($(CURR_GIT_VERSION),) # if empty then we are on main branch     
    CURR_GIT_VERSION :=MasterV0.0
endif
MAIN_VERSION_STR := $(CURR_GIT_VERSION)
FULL_VERSION_STR := $(CURR_GIT_VERSION)r$(DATE_STR).$(TIME_STR)
GLOBAL_DEFINES += VERSION_STR="$(FULL_VERSION_STR)"

GLOBAL_CFLAGS := $(GLOBAL_PROJECT_SPECIFIC_CFLAGS)
GLOBAL_ASMFLAGS := $(GLOBAL_PROJECT_SPECIFIC_ASMLAGS)
GLOBAL_LDFLAGS := $(GLOBAL_PROJECT_SPECIFIC_LDFLAGS)

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

caclulate_component_dir = $(patsubst  %/,%, $(dir $(patsubst $(APP_ROOT_DIR)/%,%,$(realpath $1 ))))

ifeq ($(findstring menuconfig,$(MAKECMDGOALS)),) #dont enter if we are bulding .config file now

    ifdef CONFIG_ARM
        ifdef CONFIG_GCC
            include $(MAKEFILES_ROOT_DIR)/gcc/gcc_arm_init.mk
        else ifdef CONFIG_GPP
            include $(MAKEFILES_ROOT_DIR)/gcc/gcc_arm_init.mk
        else ifdef CONFIG_ARMCC
            include $(MAKEFILES_ROOT_DIR)/armcc/armcc_init.mk
        endif
    else ifdef CONFIG_PIC32
        include $(MAKEFILES_ROOT_DIR)/gcc/gcc_pic32_init.mk
    else ifdef CONFIG_AVR
        include $(MAKEFILES_ROOT_DIR)/gcc/gcc_avr_init.mk
    else ifdef CONFIG_STM8
        include $(MAKEFILES_ROOT_DIR)/cxstm8/cxstm8_init.mk
    else ifdef CONFIG_HEXAGON
        include $(MAKEFILES_ROOT_DIR)/clang/clang_hexagon_init.mk
    else ifdef CONFIG_HOST
        ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
            ifdef CONFIG_MIN_GW_GCC
                include $(MAKEFILES_ROOT_DIR)/gcc/gcc_host_init.mk
            else ifdef CONFIG_MICROSOFT_COMPILER
                include $(MAKEFILES_ROOT_DIR)/microsoft_compiler/msvc_host_init.mk
            endif
        else
            include $(MAKEFILES_ROOT_DIR)/gcc/gcc_host_init.mk
        endif
    else
        $(error ---- unknown compiler ----)
    endif

endif

#########################################################
#####   components configuration includes folder :  #####  
GLOBAL_INCLUDE_DIR := $(GLOBAL_INCLUDE_DIR) $(APP_ROOT_DIR)/project_config_includes
GLOBAL_INCLUDE_DIR := $(GLOBAL_INCLUDE_DIR) $(APP_ROOT_DIR)/z_auto_generated_files

#caclulate_component_dir = $(patsubst  %/,%, $(dir $(realpath $(1) )))

COMMON_INIT_SECTION_THAT_SHOULD_RUN_ONCE = dummy_value

export

endif####
### end of section that run just one time per build
#####################################################
