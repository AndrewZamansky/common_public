

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
    COMMON_ROOT_DIR	:=	$(APP_ROOT_DIR)/../common/c
else ifneq ($(wildcard $(APP_ROOT_DIR)/../../common/c),) 
    COMMON_ROOT_DIR	:=	$(APP_ROOT_DIR)/../../common/c
else ifneq ($(wildcard $(APP_ROOT_DIR)/../../../common/c),) 
    COMMON_ROOT_DIR	:=	$(APP_ROOT_DIR)/../../../common/c
else ifneq ($(wildcard $(APP_ROOT_DIR)/../../../../common/c),) 
    COMMON_ROOT_DIR	:=	$(APP_ROOT_DIR)/../../../../common/c
else
	$(error ---- COMMON C \(common/c\) DIR NOT FOUND ----)
endif

#find root project directory  :
ifneq ($(wildcard $(APP_ROOT_DIR)/../../apps),) 
    #RELATIVE_PROJECT_ROOT_PATH	:=	
else ifneq ($(wildcard $(APP_ROOT_DIR)/../../../apps),) 
    RELATIVE_PROJECT_ROOT_PATH	:=..
else ifneq ($(wildcard $(APP_ROOT_DIR)/../../../../apps),) 
    RELATIVE_PROJECT_ROOT_PATH	:=../..
else
   $(error ---- apps  DIR NOT FOUND ----)
endif


PROJECT_NAME :=$(strip $(PROJECT_NAME))
WORKSPACE_ROOT_DIR := $(patsubst $(APP_ROOT_DIR)/%,%,$(realpath $(COMMON_ROOT_DIR)/../..))
WORKSPACE_NAME := $(notdir $(WORKSPACE_ROOT_DIR))

EXTERNAL_SOURCE_ROOT_DIR := $(patsubst %/,%,$(dir $(WORKSPACE_ROOT_DIR)))/external_source
TOOLS_ROOT_DIR := $(patsubst %/,%,$(dir $(WORKSPACE_ROOT_DIR)))/tools

include $(WORKSPACE_ROOT_DIR)/workspace_config.mk

DRIVERS_ROOT_DIR := $(COMMON_ROOT_DIR)/hw_drivers
SW_PACKAGES_ROOT_DIR	:=	$(COMMON_ROOT_DIR)/sw_packages
OS_ROOT_DIR	:=	$(COMMON_ROOT_DIR)/os

AUTO_GENERATED_FILES_DIR := $(APP_ROOT_DIR)/z_auto_generated_files
OBJ_DIR	:= $(APP_ROOT_DIR)/zOBJ
OUT_DIR	:=	$(APP_ROOT_DIR)/zOUT
OUT_DIR_HISTORY	:=	$(APP_ROOT_DIR)/zOUT_history

ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS) 	 

	#replace backslash for slash
	APP_ROOT_DIR_WINDOWS := $(subst /,\,$(APP_ROOT_DIR))
	OBJ_DIR := $(subst /,\,$(OBJ_DIR))
	OUT_DIR := $(subst /,\,$(OUT_DIR))
	AUTO_GENERATED_FILES_DIR := $(subst /,\,$(AUTO_GENERATED_FILES_DIR))
	OUT_DIR_HISTORY := $(subst /,\,$(OUT_DIR_HISTORY))
	TOOLS_ROOT_DIR := $(subst /,\,$(TOOLS_ROOT_DIR))
	TOOLS_ROOT_DIR := $(TOOLS_ROOT_DIR)\windows
	CONFIG_SEMIHOSTING_UPLOADING_DIR := $(subst /,\,$(CONFIG_SEMIHOSTING_UPLOADING_DIR))
	
	CRC32CALC	=	$(TOOLS_ROOT_DIR)\crc32\crc32.exe
    ifdef REDEFINE_MAKE_PROGRAM_DIR
        $(info  make  redefined to $(MAKE_PROGRAM_DIR)\make)
	    MAKE 	:= 	$(REDEFINE_MAKE_PROGRAM_DIR)\make
	else
	    MAKE 	:= 	$(TOOLS_ROOT_DIR)\make4.1\bin\make
    endif
	

	SHELL_GO_TO_COMMON_GIT_DIR :=cd $(WORKSPACE_ROOT_DIR)\common &
	RM		:=rmdir /S /Q
	CP		:=copy /Y
	DATE	:=date /T
	TIME	:=time /T	
else ifeq ($(findstring LINUX,$(COMPILER_HOST_OS)),LINUX) 


	SHELL_GO_TO_COMMON_GIT_DIR :=cd $(WORKSPACE_ROOT_DIR)/common ;
	MAKE 	:= 	make
	RM		:=rm -rf
	CP		:=cp -f
	DATE	:=date "+%Y-%m-%d" 
	
endif

MKDIR=mkdir	



COMPONENTS_MK := $(AUTO_GENERATED_FILES_DIR)/include_components.mk
rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))


ALL_CONFIG_FILES := $(call rwildcard,$(APP_ROOT_DIR)/,Makefile.uc.mk)
ALL_CONFIG_FILES := $(ALL_CONFIG_FILES) $(call rwildcard,$(SW_PACKAGES_ROOT_DIR)/,Makefile.uc.mk)
ALL_CONFIG_FILES := $(ALL_CONFIG_FILES) $(call rwildcard,$(DRIVERS_ROOT_DIR)/,Makefile.uc.mk)
$(info scan for uconfig.mk done )


include config.mk

####################     configuring git  ######################

GIT_DIR := $(firstword $(wildcard ./.git))
ifeq ($(findstring ./.git,$(GIT_DIR)),) 	 # if not found ./.git in $(GIT_DIR)
    $(error  error : create git repository of project (run "git init" in project directory) )
endif

CURR_GIT_BRANCH := $(shell git rev-parse --abbrev-ref HEAD 2>&1)
ifneq ($(findstring ambiguous argument 'HEAD',$(CURR_GIT_BRANCH)),) 	 # if not found $(PROJECT_NAME) in $(CURR_GIT_BRANCH)
    $(info git error  :   $(CURR_GIT_BRANCH))
    $(info maybe branch was not created after git initialization )
    $(info in this case create branch running following comands in project directory:)
    $(info git add .)
    $(info git commit -m "initial commit")
    $(error )
endif
ifeq ($(findstring $(PROJECT_NAME),$(CURR_GIT_BRANCH)),) 	 # if not found $(PROJECT_NAME) in $(CURR_GIT_BRANCH)
    $(info  error : branch names must be of type $(PROJECT_NAME) or $(PROJECT_NAME)_<branch_name>)
    $(info  in case that this git is just created run following comand in project directory:)
    $(info git branch -m $(PROJECT_NAME))
    $(error )
endif

SHELL_OUTPUT := $(shell $(SHELL_GO_TO_COMMON_GIT_DIR) git rev-parse --abbrev-ref HEAD)
ifneq ($(sort $(filter $(CURR_GIT_BRANCH),$(SHELL_OUTPUT))),$(CURR_GIT_BRANCH)) 	 
    SHELL_OUTPUT := $(shell $(SHELL_GO_TO_COMMON_GIT_DIR) git status --porcelain 2>&1)
    ERROR_MESSAGE := M 
    ifeq ($(findstring $(ERROR_MESSAGE),$(SHELL_OUTPUT)),$(ERROR_MESSAGE)) 	 
        $(info  git error : commit all changes to common git)
        $(error  )
    endif
    ERROR_MESSAGE := D 
    ifeq ($(findstring $(ERROR_MESSAGE),$(SHELL_OUTPUT)),$(ERROR_MESSAGE)) 	 
        $(info  git error : commit all changes to common git)
        $(error  )
    endif
    SHELL_OUTPUT := $(shell $(SHELL_GO_TO_COMMON_GIT_DIR) git checkout $(CURR_GIT_BRANCH) 2>&1)
    ERROR_MESSAGE :=did not match any file
    ifeq ($(findstring $(ERROR_MESSAGE),$(SHELL_OUTPUT)),$(ERROR_MESSAGE)) 	 
        $(info  git error : branch $(CURR_GIT_BRANCH) not found in common git . create it)
        $(info  in case that this git is just created run following comand in common directory:)
        $(info $(SHELL_GO_TO_COMMON_GIT_DIR) git branch $(PROJECT_NAME))
        $(error  )
    endif
endif

####################   end of  configuring git  ######################


OUTPUT_APP_NAME := out
ifdef CONFIG_OUTPUT_NAME
	OUTPUT_APP_NAME := $(CONFIG_OUTPUT_NAME)
endif

OUTPUT_CRC32 :=  $(OUT_DIR)/$(OUTPUT_APP_NAME).crc32


EMPTY:=
SPACE:= $(EMPTY) $(EMPTY)

### calculate date
DATE_STR := $(subst -,$(SPACE),$(strip $(shell $(DATE))))#replace '-' with ' '
DATE_STR := $(subst /,$(SPACE),$(DATE_STR))#replace '/' with ' '

DATE_YEAR_STR :=$(word 1,$(DATE_STR))
DATE_STR := $(patsubst 20%,%,$(DATE_YEAR_STR)) $(wordlist 2,3,$(DATE_STR))#convert year 20XX to XX and add rest of date
DATE_STR := $(subst $(SPACE),.,$(DATE_STR))#replace ' ' with '.'

### calculating version number 
TIME_STR := $(subst :,.,$(strip $(shell $(TIME))))#replace ':' with '.'
GLOBAL_DEFINES += VERSION_STR="$(DATE_STR).$(TIME_STR)"

GLOBAL_CFLAGS := $(GLOBAL_PROJECT_SPECIFIC_CFLAGS)
GLOBAL_ASMFLAGS := $(GLOBAL_PROJECT_SPECIFIC_ASMLAGS)
GLOBAL_LDFLAGS := $(GLOBAL_PROJECT_SPECIFIC_LDFLAGS)

ARCHIVE_OUTPUT := $(WORKSPACE_NAME).$(PROJECT_NAME).$(DATE_STR).7z

#####   global compiler and linker flags :  #####   
 
GLOBAL_DEFINES += CONFIG_RAM_START_ADDR=$(CONFIG_RAM_START_ADDR)

ifeq ($(findstring cortex-m,$(CONFIG_CPU_TYPE)),cortex-m)
	GLOBAL_DEFINES := $(GLOBAL_DEFINES) CORTEX_M
endif

ifeq ($(findstring cortex-m3,$(CONFIG_CPU_TYPE)),cortex-m3)
	GLOBAL_DEFINES := $(GLOBAL_DEFINES) CORTEX_M_TYPE=3
endif

ifeq ($(findstring cortex-m4,$(CONFIG_CPU_TYPE)),cortex-m4)
	GLOBAL_DEFINES := $(GLOBAL_DEFINES) CORTEX_M_TYPE=4
endif

ifeq ($(findstring YES,$(CONFIG_TEST_TASK_STACK)),YES) 	 
	GLOBAL_DEFINES := $(GLOBAL_DEFINES) TEST_TASK_STACK=1
else
	GLOBAL_DEFINES := $(GLOBAL_DEFINES) TEST_TASK_STACK=0
endif

GLOBAL_DEFINES := $(GLOBAL_DEFINES) PROJECT_NAME="$(PROJECT_NAME)"

caclulate_component_dir = $(patsubst  %/,%, $(dir $(patsubst $(APP_ROOT_DIR)/%,%,$(realpath $1 ))))

ifeq ($(findstring ARM-NONE-EABI-GCC,$(CONFIG_USE_COMPILER)),ARM-NONE-EABI-GCC) 	 
	include $(MAKEFILE_DEFS_ROOT_DIR)/gcc/gcc_arm_init.mk
else ifeq ($(findstring ARMCC,$(CONFIG_USE_COMPILER)),ARMCC) 
	include $(MAKEFILE_DEFS_ROOT_DIR)/armcc/armcc_init.mk
else ifeq ($(findstring CXSTM8,$(CONFIG_USE_COMPILER)),CXSTM8) 
	include $(MAKEFILE_DEFS_ROOT_DIR)/cxstm8/cxstm8_init.mk
else ifeq ($(findstring AVR-GCC,$(CONFIG_USE_COMPILER)),AVR-GCC) 
	include $(MAKEFILE_DEFS_ROOT_DIR)/gcc/gcc_avr_init.mk
else ifeq ($(findstring HOST-GCC,$(CONFIG_USE_COMPILER)),HOST-GCC) 
	include $(MAKEFILE_DEFS_ROOT_DIR)/gcc/gcc_host_init.mk
else
   $(error ---- unknown compiler ----)
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
