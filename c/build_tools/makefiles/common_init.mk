

#################################################################
### the following section we need to run just one time per build
ifndef COMMON_INIT_SECTION_THAT_SHOULD_RUN_ONCE

include $(MAKEFILE_DEFS_ROOT_DIR)/common_init_environment.mk


GLOBAL_PROJECT_SPECIFIC_CFLAGS :=
GLOBAL_PROJECT_SPECIFIC_ASMLAGS :=
GLOBAL_PROJECT_SPECIFIC_LDFLAGS :=
GLOBAL_DEFINES :=
GLOBAL_INCLUDE_DIR :=
SUBDIRS :=
LIBS :=
LIBRARIES_DIRS :=
LDS_PREPROCESSOR_FLAGS :=

#find "common/c" directory path :
ifneq ($(wildcard $(APP_ROOT_DIR)/../common/c),) 
    COMMON_ROOT_DIR    :=    $(APP_ROOT_DIR)/../common/c
else ifneq ($(wildcard $(APP_ROOT_DIR)/../../common/c),) 
    COMMON_ROOT_DIR    :=    $(APP_ROOT_DIR)/../../common/c
else ifneq ($(wildcard $(APP_ROOT_DIR)/../../../common/c),) 
    COMMON_ROOT_DIR    :=    $(APP_ROOT_DIR)/../../../common/c
else ifneq ($(wildcard $(APP_ROOT_DIR)/../../../../common/c),) 
    COMMON_ROOT_DIR    :=    $(APP_ROOT_DIR)/../../../../common/c
else
    $(error ---- COMMON C \(common/c\) DIR NOT FOUND ----)
endif

#find root project directory  :
ifneq ($(wildcard $(APP_ROOT_DIR)/../../apps),) 
    #RELATIVE_PROJECT_ROOT_PATH    :=    
else ifneq ($(wildcard $(APP_ROOT_DIR)/../../../apps),) 
    RELATIVE_PROJECT_ROOT_PATH    :=..
else ifneq ($(wildcard $(APP_ROOT_DIR)/../../../../apps),) 
    RELATIVE_PROJECT_ROOT_PATH    :=../..
else
   $(error ---- apps  DIR NOT FOUND ----)
endif

WORKSPACE_ROOT_DIR := $(patsubst $(APP_ROOT_DIR)/%,%,$(realpath $(COMMON_ROOT_DIR)/../..))
WORKSPACE_NAME := $(notdir $(WORKSPACE_ROOT_DIR))

EXTERNAL_SOURCE_ROOT_DIR := $(patsubst %/,%,$(dir $(WORKSPACE_ROOT_DIR)))/external_source
ifeq ("$(wildcard $(EXTERNAL_SOURCE_ROOT_DIR))","")
    $(info directory $(EXTERNAL_SOURCE_ROOT_DIR) dont exists . create it .)
    $(error )
endif
TOOLS_ROOT_DIR := $(patsubst %/,%,$(dir $(WORKSPACE_ROOT_DIR)))/tools
ifeq ("$(wildcard $(TOOLS_ROOT_DIR))","")
    $(info directory $(TOOLS_ROOT_DIR) dont exists . create it .)
    $(error )
endif

include $(WORKSPACE_ROOT_DIR)/workspace_config.mk

DRIVERS_ROOT_DIR := $(COMMON_ROOT_DIR)/hw_drivers
SW_PACKAGES_ROOT_DIR    :=    $(COMMON_ROOT_DIR)/sw_packages
OS_ROOT_DIR    :=    $(COMMON_ROOT_DIR)/os

AUTO_GENERATED_FILES_DIR := $(APP_ROOT_DIR)/z_auto_generated_files
OBJ_DIR    := $(APP_ROOT_DIR)/zOBJ
OUT_DIR    :=    $(APP_ROOT_DIR)/zOUT
OUT_DIR_HISTORY    :=    $(APP_ROOT_DIR)/zOUT_history
DEFAULT_GIT_IGNORE_FILE    :=    $(COMMON_ROOT_DIR)/build_tools/default.project.gitignore
CURRENT_GIT_IGNORE_FILE    :=    $(APP_ROOT_DIR)/.gitignore

MKDIR=mkdir    

COMMON_DIR = $(WORKSPACE_ROOT_DIR)/common

EMPTY:=
SPACE:= $(EMPTY) $(EMPTY)

ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)      

    #replace backslash for slash
    APP_ROOT_DIR_WINDOWS := $(subst /,\,$(APP_ROOT_DIR))
    OBJ_DIR := $(subst /,\,$(OBJ_DIR))
    OUT_DIR := $(subst /,\,$(OUT_DIR))
    AUTO_GENERATED_FILES_DIR := $(subst /,\,$(AUTO_GENERATED_FILES_DIR))
    OUT_DIR_HISTORY := $(subst /,\,$(OUT_DIR_HISTORY))
    TOOLS_ROOT_DIR := $(subst /,\,$(TOOLS_ROOT_DIR))
    TOOLS_ROOT_DIR := $(TOOLS_ROOT_DIR)\windows
    COMMON_DIR := $(subst /,\,$(COMMON_DIR))
    DEFAULT_GIT_IGNORE_FILE := $(subst /,\,$(DEFAULT_GIT_IGNORE_FILE))
    CURRENT_GIT_IGNORE_FILE := $(subst /,\,$(CURRENT_GIT_IGNORE_FILE))

    CRC32CALC    =    $(TOOLS_ROOT_DIR)\crc32\crc32.exe

    ifdef REDEFINE_MAKE_PROGRAM_DIR
        $(info  make  redefined to $(REDEFINE_MAKE_PROGRAM_DIR)\make)
        ifeq ("$(wildcard $(REDEFINE_MAKE_PROGRAM_DIR))","")
            $(info make path $(REDEFINE_MAKE_PROGRAM_DIR) dont exists)
            $(info to use default make location remove/comment REDEFINE_MAKE_PROGRAM_DIR variable in  $(REDEFINE_MAKE_PROGRAM_DIR)/workspace_config.mk )
            $(info you can set customized make utility path in REDEFINE_MAKE_PROGRAM_DIR variable in $(REDEFINE_MAKE_PROGRAM_DIR)/workspace_config.mk )
            $(error )
        else
            MAKE_DIR  :=$(REDEFINE_MAKE_PROGRAM_DIR)
        endif
    else
        $(info  looking for GNU make in default location)
        MAKE_DIR     :=$(TOOLS_ROOT_DIR)\make\make4.1
        ifeq ("$(wildcard $(MAKE_DIR))","")
            $(info !--- make path $(MAKE_DIR) dont exists )
            $(info !--- download GNU make version 4.1 and unpack it to $(MAKE_DIR)  )
            $(info !--- make sure that bin directory is located in $(MAKE_DIR)\  after unpacking  .  )
            $(info !--- you can set customized make utility path in REDEFINE_MAKE_PROGRAM_DIR variable in $(REDEFINE_MAKE_PROGRAM_DIR)/workspace_config.mk )
            $(error )
        endif
        MAKE_DIR := $(MAKE_DIR)/bin
    endif


    MAKE :=$(MAKE_DIR)\make

    CONFIG_SEMIHOSTING_UPLOADING_DIR :=c:\Temp
    ifeq ($(wildcard $(CONFIG_SEMIHOSTING_UPLOADING_DIR)),)         #if $(CONFIG_SEMIHOSTING_UPLOADING_DIR) dont exists then $(wildcard $(CONFIG_SEMIHOSTING_UPLOADING_DIR)) will produce empty string 
       DUMMY:=$(shell $(MKDIR)  $(CONFIG_SEMIHOSTING_UPLOADING_DIR)) # create   $(CONFIG_SEMIHOSTING_UPLOADING_DIR)
    endif

    COMMON_PARTITION := $(firstword $(subst :, ,$(COMMON_DIR))):
    SHELL_GO_TO_COMMON_GIT_DIR :=cd $(COMMON_DIR) & $(COMMON_PARTITION) & 
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
    TOOLS_ROOT_DIR := $(TOOLS_ROOT_DIR)/linux

    CRC32CALC    =    cksum 

    SHELL_GO_TO_COMMON_GIT_DIR :=cd $(COMMON_DIR) ;
    MAKE     :=     make
    RM        :=rm -rf
    CP        :=cp -f
    DATE    :=$(shell date "+%Y-%m-%d") 
    TIME    :=$(shell date "+%H:%M") 
    SHELL_CMD_DELIMITER = ;
    CD :=  cd

endif


ifeq ($(findstring menuconfig,$(MAKECMDGOALS)),) 

include .config

PROJECT_NAME :=$(patsubst "%",%,$(CONFIG_PROJECT_NAME))
ifeq ($(PROJECT_NAME),)      # if $(PROJECT_NAME) is empty
    $(info error : project have to be named set CONFIG_PROJECT_NAME in .config or using menuconfig)
    $(error )
endif
$(info ---- project directory : $(APP_ROOT_DIR) ---- )
$(info ---- project name as declared in .config : $(PROJECT_NAME) ---- )


####################     configuring git  ######################

GIT_DIR := $(firstword $(wildcard ./.git))
ifeq ($(findstring ./.git,$(GIT_DIR)),)      # if not found ./.git in $(GIT_DIR)
    $(info !--- error : create git repository of project . for example by running following command : )
    $(info !--- $(CD) $(APP_ROOT_DIR) $(SHELL_CMD_DELIMITER) git init $(SHELL_CMD_DELIMITER) $(CP) $(DEFAULT_GIT_IGNORE_FILE) $(CURRENT_GIT_IGNORE_FILE))
    $(error )
endif

CURR_GIT_BRANCH := $(shell git rev-parse --abbrev-ref HEAD 2>&1)
CURR_GIT_BRANCH := $(patsubst heads/%,%,$(CURR_GIT_BRANCH))#removing heads/ if exists
ifneq ($(findstring ambiguous argument 'HEAD',$(CURR_GIT_BRANCH)),)      # if not found $(PROJECT_NAME) in $(CURR_GIT_BRANCH)
    $(info !--- git error  :   $(CURR_GIT_BRANCH))
    $(info !--- maybe branch was not created after git initialization )
    $(info !--- in this case create branch you can run following comands to add all files in ):)
    $(info !--- $(CD) $(APP_ROOT_DIR) $(SHELL_CMD_DELIMITER) git add . & git commit -m "initial commit")
    $(error )
endif
ifeq ($(findstring $(PROJECT_NAME),$(CURR_GIT_BRANCH)),)      # if not found $(PROJECT_NAME) in $(CURR_GIT_BRANCH)
    $(info !--- error : branch names must be of type $(PROJECT_NAME) or $(PROJECT_NAME)_<branch_name>)
    $(info !--- but current branch name is $(CURR_GIT_BRANCH))
    $(info !--- in case that this git is just created run following comand  :)
    $(info !--- $(CD) $(APP_ROOT_DIR) $(SHELL_CMD_DELIMITER) git branch -m $(PROJECT_NAME))
    $(error )
endif

CURR_COMMON_GIT_BRANCH := $(shell $(SHELL_GO_TO_COMMON_GIT_DIR) git rev-parse --abbrev-ref HEAD)
CURR_COMMON_GIT_BRANCH := $(patsubst heads/%,%,$(CURR_COMMON_GIT_BRANCH))#removing heads/ if exists
ifneq ($(sort $(filter $(CURR_GIT_BRANCH),$(CURR_COMMON_GIT_BRANCH))),$(CURR_GIT_BRANCH))#if  $(CURR_GIT_BRANCH) is in $(SHELL_OUTPUT)
    SHELL_OUTPUT := $(shell $(SHELL_GO_TO_COMMON_GIT_DIR) git status --porcelain 2>&1)
    ERROR_MESSAGE := M 
    ifeq ($(findstring $(ERROR_MESSAGE),$(SHELL_OUTPUT)),$(ERROR_MESSAGE))
        $(info  !--- git error : commit all changes to common git($(COMMON_DIR)) before changing branch or project)
        $(info  !--- current application git branch :   $(CURR_GIT_BRANCH) )
        $(info  !--- current common git branch :   $(CURR_COMMON_GIT_BRANCH) )
        $(error  )
    endif
    ERROR_MESSAGE := D #??
    ifeq ($(findstring $(ERROR_MESSAGE),$(SHELL_OUTPUT)),$(ERROR_MESSAGE))
        $(info  !--- git error : commit all changes to common git)
        $(error  )
    endif
   #for now we are doing manual checkout
   #SHELL_OUTPUT := $(shell $(SHELL_GO_TO_COMMON_GIT_DIR) git checkout $(CURR_GIT_BRANCH) 2>&1)
    SHELL_OUTPUT := $(shell $(SHELL_GO_TO_COMMON_GIT_DIR) git branch 2>&1)
    CURR_GIT_BRANCH:=$(patsubst heads/%,%,$(CURR_GIT_BRANCH))
    ifneq ($(sort $(filter $(CURR_GIT_BRANCH),$(SHELL_OUTPUT))),$(CURR_GIT_BRANCH))#if  $(CURR_GIT_BRANCH) is in $(SHELL_OUTPUT)
        $(info !--- git error : branch $(CURR_GIT_BRANCH) not found in common git . create it)
        $(info !--- in case that this git is just created run following comand in common directory:)
        $(info !--- $(SHELL_GO_TO_COMMON_GIT_DIR) git branch $(PROJECT_NAME))
        $(error  )
    else
        $(info !--- checkout $(CURR_GIT_BRANCH) manually in common git)
        $(info !--- you can run following comand in common directory:)
        $(info !--- $(SHELL_GO_TO_COMMON_GIT_DIR) git checkout $(CURR_GIT_BRANCH))
        $(error  )
    endif
endif

####################   end of  configuring git  ######################

endif


COMPONENTS_MK := $(AUTO_GENERATED_FILES_DIR)/include_components.mk
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
    RAM_START_ADDR:=$(CONFIG_INTERNAL_SRAM_START_ADDR)
    RAM_SIZE:=$(CONFIG_INTERNAL_SRAM_SIZE)
endif
ifdef CONFIG_DATA_LOCATION_DDR
    RAM_START_ADDR:=$(CONFIG_DDR_START_ADDR)
    RAM_SIZE:=$(CONFIG_DDR_SIZE)
endif

ifdef CONFIG_CODE_LOCATION_INTERNAL_SRAM
    ROM_START_ADDR:=$(CONFIG_INTERNAL_SRAM_START_ADDR)
    ROM_SIZE:=$(CONFIG_INTERNAL_SRAM_SIZE)
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
    ROM_START_ADDR:=$(CONFIG_INTERNAL_FLASH_START_ADDR)
    ROM_SIZE:=$(CONFIG_INTERNAL_FLASH_SIZE)
endif
ifdef CONFIG_CODE_LOCATION_EXTERNAL_FLASH
    ROM_START_ADDR:=$(CONFIG_EXTERNAL_FLASH_START_ADDR)
    ROM_SIZE:=$(CONFIG_EXTERNAL_FLASH_SIZE)
endif

GLOBAL_DEFINES += RAM_START_ADDR=$(RAM_START_ADDR)

caclulate_component_dir = $(patsubst  %/,%, $(dir $(patsubst $(APP_ROOT_DIR)/%,%,$(realpath $1 ))))

ifeq ($(findstring menuconfig,$(MAKECMDGOALS)),)

ifdef CONFIG_ARM
    ifdef CONFIG_GCC
        include $(MAKEFILE_DEFS_ROOT_DIR)/gcc/gcc_arm_init.mk
    else ifdef CONFIG_GPP
        include $(MAKEFILE_DEFS_ROOT_DIR)/gcc/gcc_arm_init.mk
    else ifdef CONFIG_ARMCC
        include $(MAKEFILE_DEFS_ROOT_DIR)/armcc/armcc_init.mk
    endif
else ifdef CONFIG_AVR
    include $(MAKEFILE_DEFS_ROOT_DIR)/gcc/gcc_avr_init.mk
else ifdef CONFIG_STM8
    include $(MAKEFILE_DEFS_ROOT_DIR)/cxstm8/cxstm8_init.mk
else ifdef CONFIG_HOST
    include $(MAKEFILE_DEFS_ROOT_DIR)/gcc/gcc_host_init.mk
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
