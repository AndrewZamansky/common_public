
#test that arguments are set correctly 
ifeq ($(SEARCHED_TOOL),)
    $(info err: SEARCHED_TOOL (= utility name) should be set before including this file)
    $(call exit,1)
endif
ifeq ($(SEARCHED_DIR_VARIABLE),)
    $(info err: SEARCHED_DIR_VARIABLE (= name of variable that will contain found directory))
    $(info ---: should be set before including this file)
    $(call exit,1)
endif
ifeq ($(MANUALLY_DEFINED_DIR_VARIABLE),)
    $(info err: MANUALLY_DEFINED_DIR_VARIABLE (= name of variable that may contains user defined tool directory))
    $(info ---: should be set before including this file)
    $(call exit,1)
endif
ifeq ($(TEST_FILE_IN_SEARCHED_DIR),)
    $(info err: TEST_FILE_IN_SEARCHED_DIR (= test file that will verify directory structure)
    $(info ---: should be set before including this file)
    $(call exit,1)
endif

TEMP := $(GLOBAL_MANUALLY_DEFINED_TOOL_DIR_LIST)
GLOBAL_MANUALLY_DEFINED_TOOL_DIR_LIST := $(TEMP) $(MANUALLY_DEFINED_DIR_VARIABLE)

FOUND_ROOT_DIR:=
MANUALLY_DEFINED_DIR_VARIABLE:=$(strip $(MANUALLY_DEFINED_DIR_VARIABLE))
REDEFINED_ROOT_DIR:=$($(MANUALLY_DEFINED_DIR_VARIABLE))
REDEFINED_ROOT_DIR:=$(strip $(REDEFINED_ROOT_DIR))


ifdef _EXPORTED_$(MANUALLY_DEFINED_DIR_VARIABLE)
    $(info err: variable _EXPORTED_$(MANUALLY_DEFINED_DIR_VARIABLE) exists in $(PARENT_OF_COMMON_PUBLIC_DIR)/workspace_config.mk .)
    $(info ---: it looks like '$(SEARCHED_TOOL)' tool still was not defined in your workspace.)
    $(info ---: change _EXPORTED_$(MANUALLY_DEFINED_DIR_VARIABLE) in $(PARENT_OF_COMMON_PUBLIC_DIR)/workspace_config.mk :)
    $(info ---: if '$(SEARCHED_TOOL)' is installed system-wide : remove/comment _EXPORTED_$(MANUALLY_DEFINED_DIR_VARIABLE) variable .)
    $(info ---: else : remove _EXPORTED_ prefix of variable and set the correct path to '$(SEARCHED_TOOL)' tool .)
    $(call exit,1)
endif

SPACE:=
SPACE:=$(SPACE) $(SPACE)

ifeq ($(REDEFINED_ROOT_DIR),)
    $(info )
    $(info ---- directory for $(SEARCHED_TOOL) was not manually)
    $(info ---- defined in $(PARENT_OF_COMMON_PUBLIC_DIR)/workspace_config.mk/$(MANUALLY_DEFINED_DIR_VARIABLE))
    $(info )
else
    $(info )
    $(info ---- trying to use $(SEARCHED_TOOL) located in directory)
    $(info ---- defined by $(PARENT_OF_COMMON_PUBLIC_DIR)/workspace_config.mk/$(MANUALLY_DEFINED_DIR_VARIABLE))
    $(info )
    
    #add escape char to spaces(needed for correct work of 'wildcard' function)
    REDEFINED_ROOT_DIR_FMT := $(subst $(SPACE),\ ,$(REDEFINED_ROOT_DIR))
    ifeq ($(wildcard $(REDEFINED_ROOT_DIR_FMT)),)
        $(info err: directory $(REDEFINED_ROOT_DIR)   defined in $(PARENT_OF_COMMON_PUBLIC_DIR)/workspace_config.mk/$(MANUALLY_DEFINED_DIR_VARIABLE) was not found)
        $(info ---: redefine $(PARENT_OF_COMMON_PUBLIC_DIR)/workspace_config.mk/$(MANUALLY_DEFINED_DIR_VARIABLE))
        $(info ---: OR remove/comment $(PARENT_OF_COMMON_PUBLIC_DIR)/workspace_config.mk/$(MANUALLY_DEFINED_DIR_VARIABLE) if you want to use system  $(SEARCHED_TOOL))
        $(call exit,1)
    else
        #add escape char to spaces(needed for correct work of 'wildcard' function)
        TEST_FILE_IN_SEARCHED_DIR_FMT := $(subst $(SPACE),\ ,$(REDEFINED_ROOT_DIR)/$(TEST_FILE_IN_SEARCHED_DIR))
        ifeq ($(wildcard $(TEST_FILE_IN_SEARCHED_DIR_FMT)),)
            $(info err: failed to use $(SEARCHED_TOOL) located in directory defined by $(PARENT_OF_COMMON_PUBLIC_DIR)/workspace_config.mk/$(MANUALLY_DEFINED_DIR_VARIABLE))
            $(info ---: $(PARENT_OF_COMMON_PUBLIC_DIR)/workspace_config.mk/$(MANUALLY_DEFINED_DIR_VARIABLE) = $(REDEFINED_ROOT_DIR))
            $(info ---: file $(TEST_FILE_IN_SEARCHED_DIR) not found in $(REDEFINED_ROOT_DIR))
            $(info ---: if you want to use current value in $(MANUALLY_DEFINED_DIR_VARIABLE) then install $(SEARCHED_TOOL) into $(REDEFINED_ROOT_DIR))
            $(info ---: and make sure that  $(TEST_FILE_IN_SEARCHED_DIR)  exists in $(REDEFINED_ROOT_DIR))            
            $(info ---: OR redefine $(PARENT_OF_COMMON_PUBLIC_DIR)/workspace_config.mk/$(MANUALLY_DEFINED_DIR_VARIABLE) to valid $(SEARCHED_TOOL) directory) 
            $(info ---: OR remove/comment $(PARENT_OF_COMMON_PUBLIC_DIR)/workspace_config.mk/$(MANUALLY_DEFINED_DIR_VARIABLE) if you want to use system  $(SEARCHED_TOOL))
            $(call exit,1)
        else
            $(info ---- using $(SEARCHED_TOOL) located in $(REDEFINED_ROOT_DIR))
            $(info )
            FOUND_ROOT_DIR:=$(REDEFINED_ROOT_DIR)
        endif
    endif
endif

ifeq ($(FOUND_ROOT_DIR),)
    $(info ---- trying system $(SEARCHED_TOOL) ...)
    FOUND_TOOL_SYSTEM_DIR :=
    ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
        TOOL_EXISTANCE_CHECK :=$(shell where /f $(SEARCHED_TOOL) 2>&1)
        #$(info dbg : where /f $(SEARCHED_TOOL))
        #$(info dbg : $(TOOL_EXISTANCE_CHECK))
        ### remove " mark and replace space in directory names by ? sign ##
        TOOL_EXISTANCE_CHECK_FMT :=$(subst "$(SPACE),"*,$(TOOL_EXISTANCE_CHECK))
        TOOL_EXISTANCE_CHECK_FMT :=$(subst $(SPACE),?,$(TOOL_EXISTANCE_CHECK_FMT))
        TOOL_EXISTANCE_CHECK_FMT :=$(subst *,$(SPACE),$(TOOL_EXISTANCE_CHECK_FMT))
        TOOL_EXISTANCE_CHECK_FMT :=$(subst ",,$(TOOL_EXISTANCE_CHECK_FMT))
        ### end of removal of # and replacing  space in directory names by ? sign ##
        FOUND_TOOL_CHECK_FMT :=$(filter %$(TEST_FILE_IN_SEARCHED_DIR),$(TOOL_EXISTANCE_CHECK_FMT))
        #$(info dbg : FOUND_TOOL_CHECK_FMT = $(FOUND_TOOL_CHECK_FMT))
        ifneq ($(FOUND_TOOL_CHECK_FMT),)
            FOUND_TOOL_CHECK_FMT :=$(filter %$(FOUND_TOOL_CHECK_FMT),$(TOOL_EXISTANCE_CHECK_FMT))
            FOUND_TOOL_SYSTEM_DIR :=$(subst \$(TEST_FILE_IN_SEARCHED_DIR),,$(FOUND_TOOL_CHECK_FMT))
            FOUND_TOOL_SYSTEM_DIR :=$(subst ?,$(SPACE),$(FOUND_TOOL_SYSTEM_DIR))
        endif
    else ifeq ($(findstring LINUX,$(COMPILER_HOST_OS)),LINUX)
        TOOL_EXISTANCE_CHECK := $(shell whereis $(SEARCHED_TOOL))
        FOUND_TOOL_CHECK :=$(filter %$(TEST_FILE_IN_SEARCHED_DIR),$(TOOL_EXISTANCE_CHECK_FMT))
        FOUND_TOOL_CHECK :=$(firstword $(FOUND_TOOL_CHECK))
        FOUND_TOOL_SYSTEM_DIR :=$(subst /$(TEST_FILE_IN_SEARCHED_DIR),,$(FOUND_TOOL_CHECK))
    endif
    
    #$(info dbg : FOUND_TOOL_SYSTEM_DIR = $(FOUND_TOOL_SYSTEM_DIR))
    ifneq ($(FOUND_TOOL_SYSTEM_DIR),)
        FOUND_ROOT_DIR:=$(FOUND_TOOL_SYSTEM_DIR)
        $(info ---- system $(SEARCHED_TOOL) found...)
        $(info )
    else
        $(info err: system $(SEARCHED_TOOL) not found) 
        $(info ---: install $(SEARCHED_TOOL) and make sure it was added to system path [or PATH in windows] variable by running )
        $(info ---: linux shell : whereis $(SEARCHED_TOOL)  )
        $(info ---: windows shell : where $(SEARCHED_TOOL)  )         
        $(info ---: make sure that you see <SOME_PATH>/$(TEST_FILE_IN_SEARCHED_DIR) in the output  )         
        $(info ---: OR redefine $(PARENT_OF_COMMON_PUBLIC_DIR)/workspace_config.mk/$(MANUALLY_DEFINED_DIR_VARIABLE) to valid $(SEARCHED_TOOL) directory) 
        $(info ---: and make sure that  $(TEST_FILE_IN_SEARCHED_DIR)  exists in that directory)            
        $(call exit,1)
    endif
endif

$(SEARCHED_DIR_VARIABLE):=$(FOUND_ROOT_DIR)

#clear arguments for next function usage
SEARCHED_TOOL:=
SEARCHED_DIR_VARIABLE:=
MANUALLY_DEFINED_DIR_VARIABLE:=
TEST_FILE_IN_SEARCHED_DIR:=
