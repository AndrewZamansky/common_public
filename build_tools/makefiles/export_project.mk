
include $(MAKEFILES_ROOT_DIR)/common.mk

include $(COMPONENTS_MK)
include $(ADDITIONAL_INIT_FILE)#should be after include of $(COMPONENTS_MK)

TOOLS_DIR :=$(PARENT_OF_COMMON_PUBLIC_DIR)/../tools
DEFAULT_MAKE_PROGRAM_DIR :=$(TOOLS_DIR)/windows/make/make4.1
DEFAULT_KCONFIG_DIR  = $(TOOLS_DIR)/windows/kconfig/kconfig3.12.0 

TOOL_DIR_LIST := $(GLOBAL_MANUALLY_DEFINED_TOOL_DIR_LIST)
REMOVE := REDEFINE_MAKE_PROGRAM_DIR REDEFINE_KCONFIG_DIR REDEFINE_7ZIP_DIR
TOOL_DIR_LIST := $(filter-out $(REMOVE),$(TOOL_DIR_LIST))
# remove duplicates :
TOOL_DIR_LIST := $(sort $(TOOL_DIR_LIST))

EXPORTS += $(APP_ROOT_DIR)
EXPORTS += $(COMMON_PUBLIC_DIR)
EXPORTS += $(DEFAULT_MAKE_PROGRAM_DIR)
EXPORTS += $(DEFAULT_KCONFIG_DIR)

USAGE_STR := make export_project EXPORT_TOOLS=(0/1) EXPORT_EXTERNAL_SRC=(0/1)

ifeq ($(EXPORT_TOOLS),1)
    TOOLS_EXPORT_DIRS := $(foreach i, $(TOOL_DIR_LIST), $($(i)))
else ifeq ($(EXPORT_TOOLS),0)
    TOOLS_EXPORT_DIRS :=
    $(info ---- not exporting all required tools)
else
    $(info err: EXPORT_TOOLS argument is required.)
    $(info ---: usage : $(USAGE_STR))
    $(call exit,1)
endif
EXPORTS += $(TOOLS_EXPORT_DIRS)

ifeq ($(EXPORT_EXTERNAL_SRC),1)
    EXPORT_EXTERNAL_SRC_DIRS := $(EXTERNAL_SRC_GIT_DIRS) $(EXTERNAL_SRC_DIRS)
else ifeq ($(EXPORT_EXTERNAL_SRC),0)
    EXPORT_EXTERNAL_SRC_DIRS :=
    $(info ---- not exporting all required tools)
else
    $(info err: EXPORT_EXTERNAL_SRC argument is required.)
    $(info ---: usage : $(USAGE_STR))
    $(call exit,1)
endif
EXPORTS += $(EXPORT_EXTERNAL_SRC_DIRS)

EXPORTS := $(sort $(EXPORTS))
EXPORTS :=$(realpath $(EXPORTS))
# remove prefix that appear on some machines :
EXPORTS :=$(patsubst $(APP_ROOT_DIR)/%,%,$(EXPORTS))

# remove common_private from export . it should be sent separatly and encrypted
EXPORTS := $(patsubst %/common_private,,$(EXPORTS))

OUTPUT_ARCHIVE_DIR :=$(realpath $(PARENT_OF_COMMON_PUBLIC_DIR)/../..)
OUTPUT_ARCHIVE_DIR :=$(patsubst %/,%,$(OUTPUT_ARCHIVE_DIR))
# remove prefix that appear on some machines :
OUTPUT_ARCHIVE_DIR :=$(patsubst $(APP_ROOT_DIR)/%,%,$(OUTPUT_ARCHIVE_DIR))

# make all files relative to archive location
EXPORTS :=$(patsubst $(OUTPUT_ARCHIVE_DIR)/%,%,$(EXPORTS))

OUTPUT_TAR :=$(PROJECT_NAME).tar
FINALL_OUTPUT :=$(PROJECT_NAME).tar.xz


TMP :=$(realpath $(PARENT_OF_COMMON_PUBLIC_DIR))/workspace_config.mk.exported
# remove prefix that appear on some machines :
NEW_WORKSPACE_CONFIG :=$(patsubst $(APP_ROOT_DIR)/%,%,$(TMP))

# workspace_config.mk.exported still not exists, so it 
# should be added AFTER execution of  $(realpath $(EXPORTS))
EXPORTS += $(patsubst $(OUTPUT_ARCHIVE_DIR)/%,%,$(NEW_WORKSPACE_CONFIG))
 
ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
    ### test for existence of make and put its directory name in 7ZIP_DIR #####
    SEARCHED_TOOL:=7zip
    SEARCHED_DIR_VARIABLE:=7ZIP_DIR
    MANUALLY_DEFINED_DIR_VARIABLE:=REDEFINE_7ZIP_DIR
    TEST_FILE_IN_SEARCHED_DIR:=7z.exe
    include $(MAKEFILES_INC_FUNC_DIR)/tool_existence_check.mk
    ####### end of tool existence test #####

    SHELL_OUT :=$(shell "$(7ZIP_DIR)/7z.exe")
    ifneq ($(findstring 16.04,$(SHELL_OUT)),)
        $(info ---- 7zip version 16.04 found)
    else ifneq ($(findstring 17.01,$(SHELL_OUT)),)
        $(info ---- 7zip version 17.01 found)
    else
        $(info err: unsupported version of 7zip)
        $(info ---: install one of following versions : 16.04 17.01)
        $(info ---: and update/remove REDEFINE_7ZIP_DIR)
        $(call exit,1)
    endif
    
    EXPORTS := $(subst /,\,$(EXPORTS))

    CREATE_TAR_CMD := cd /D $(OUTPUT_ARCHIVE_DIR)/ & del /Q $(OUTPUT_TAR) &
    CREATE_TAR_CMD += echo start time : & time /T & 
    CREATE_TAR_CMD += "$(7ZIP_DIR)/7z.exe"
    CREATE_TAR_CMD += a -ttar -xr!zOBJ -xr!zOUT -xr!zOUT_history
    CREATE_TAR_CMD += $(OUTPUT_TAR) $(EXPORTS)
    
    CONTENT := \#
    $(shell echo $(CONTENT) > $(NEW_WORKSPACE_CONFIG))
    CONTENT := \# you can use $$(PARENT_OF_COMMON_PUBLIC_DIR)  variable
    $(shell echo $(CONTENT) >> $(NEW_WORKSPACE_CONFIG))
    CONTENT := \# to define path relative to current uCProject directory
    $(shell echo $(CONTENT) >> $(NEW_WORKSPACE_CONFIG))
    CONTENT := \#
    $(shell echo $(CONTENT) >> $(NEW_WORKSPACE_CONFIG))
    $(shell echo.>> $(NEW_WORKSPACE_CONFIG))# add 1 empty line
    

    CREATE_XZ_CMD := cd /D $(OUTPUT_ARCHIVE_DIR)/ & del /Q $(FINALL_OUTPUT) &
    CREATE_XZ_CMD += echo start time : & time /T & 
    CREATE_XZ_CMD += "$(7ZIP_DIR)/7z.exe"
    CREATE_XZ_CMD += a -txz -mx9 $(FINALL_OUTPUT) $(OUTPUT_TAR) &
    CREATE_XZ_CMD += del /Q $(OUTPUT_TAR)
    
endif


# REDEFINE_7ZIP_DIR was added after current 7-zip tool check

# creating new_workspace_config.mk :

# replace prefix $(PARENT_OF_COMMON_PUBLIC_DIR)
# by string "$(PARENT_OF_COMMON_PUBLIC_DIR)" in new workspace_config.mk
PREFIX:=$(PARENT_OF_COMMON_PUBLIC_DIR)
replace_prefix =$(patsubst $(PREFIX)/%,$$(PARENT_OF_COMMON_PUBLIC_DIR)/%,$(1))

echo_cmd = echo $(1)  = $(call replace_prefix,$(2)) >> $(NEW_WORKSPACE_CONFIG)
add_path2 = $(if $(1), $(shell $(call echo_cmd,$(1),$(2)) ), )
ifeq ($(EXPORT_TOOLS),1)
    add_path1 = $(if $($(1)), $(call add_path2,$(1),$($(1))), )
else ifeq ($(EXPORT_TOOLS),0)
    add_path1 = $(if $($(1)), $(call add_path2,_EXPORTED_$(1),$($(1))), )
endif

$(call add_path2,REDEFINE_MAKE_PROGRAM_DIR,$(DEFAULT_MAKE_PROGRAM_DIR))
$(call add_path2,REDEFINE_KCONFIG_DIR,$(DEFAULT_KCONFIG_DIR))

$(foreach i, $(TOOL_DIR_LIST), $(call add_path1,$(i)))

CONTENT :=\#REDEFINE_OUTPUT_DIR  = C:\work\tmp
$(shell echo $(CONTENT) >> $(NEW_WORKSPACE_CONFIG))


$(info ---- EXPORTS = $(EXPORTS))
#$(call exit,1)

create_tar :
	$(CREATE_TAR_CMD)
    
create_xz : create_tar
	$(CREATE_XZ_CMD)

export_project : create_xz
	$(info --- export done)

.PHONY: create_xz create_tar
