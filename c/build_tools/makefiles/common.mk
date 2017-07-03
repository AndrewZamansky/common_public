#################################################################
### SHELL variable cannot be exported so we have to assign it everytime here
###

SHELL := $(CALCULATED_SHELL)

######################################

EMPTY:=
SPACE:= $(EMPTY) $(EMPTY)

current_dir :=$(dir $(abspath $(firstword $(MAKEFILE_LIST))))
abspath_fix =$(patsubst $(current_dir)%,%,$(abspath $(1) ))

caclulate_dir = $(patsubst  %/,%, $(dir $(abspath $(1) )))

ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS) 	 
    ifeq ($(findstring 3.81,$(MAKE_VERSION)),3.81) 	 
        caclulate_dir = $(patsubst  %/,%, $(dir $(call abspath_fix,$(1) )))
    endif
    SHELL_CMD_DELIMITER = &
else ifeq ($(findstring LINUX,$(COMPILER_HOST_OS)),LINUX)
    SHELL_CMD_DELIMITER = ;
endif


define ADD_TO_GLOBAL_INCLUDE_PATH =
    $(eval GLOBAL_INCLUDE_DIR := $(GLOBAL_INCLUDE_DIR) $(1))
endef
#usage :
#    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , path/to/include/dir )

define ADD_TO_GLOBAL_DEFINES =
    $(eval GLOBAL_DEFINES := $(GLOBAL_DEFINES) $(1))
endef
#usage :
#    DUMMY := $(call ADD_TO_GLOBAL_DEFINES , var )
#    DUMMY := $(call ADD_TO_GLOBAL_DEFINES , var=val )

define ADD_TO_GLOBAL_LIBRARIES =
    $(eval GLOBAL_LIBS := $(GLOBAL_LIBS) $(1))
endef
#usage :
#    DUMMY := $(call ADD_TO_GLOBAL_LIBRARIES , var )

define ADD_TO_GLOBAL_LIBRARIES_PATH =
    $(eval GLOBAL_LIBS_PATH := $(GLOBAL_LIBS_PATH) $(1))
endef
#usage :
#    DUMMY := $(call ADD_TO_GLOBAL_LIBRARIES_PATH , var )

define ADD_TO_GLOBAL_ARCHIVES =
    $(eval GLOBAL_ARCHIVES := $(GLOBAL_ARCHIVES) $(1))
endef
#usage :
#    DUMMY := $(call ADD_TO_GLOBAL_ARCHIVES , var )
