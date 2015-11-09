
CURR_WORKING_DIR=$(subst \,/,$(WORKING_DIR))

SPACE:= $(EMPTY) $(EMPTY)

WORKSPACE_ROOT_DIR := $(firstword $(subst /common/c/,$(SPACE),$(CURR_WORKING_DIR)))

include $(WORKSPACE_ROOT_DIR)/workspace_config.mk

MAKEFILE_DEFS_ROOT_DIR = $(WORKSPACE_ROOT_DIR)/common/c/build_tools/makefiles
TOOLS_ROOT_DIR := $(patsubst %/,%,$(dir $(WORKSPACE_ROOT_DIR)))/tools

include $(MAKEFILE_DEFS_ROOT_DIR)/common_init_environment.mk

include $(CURR_WORKING_DIR)/docs_lookup_words.mk


ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS) 	 

SEARCH_UTILITY =$(TOOLS_ROOT_DIR)\windows\SearchMyFiles\SearchMyFiles.exe

default:
	@echo WORKING_DIR=$(CURR_WORKING_DIR)
	start "" $(SEARCH_UTILITY) /BaseFolder "$(DOCS_DIR)" /FilesWildcard "$(DOCS_LOOKUP_WORDS)" /FindFiles 1 /StartSearch

else ifeq ($(findstring LINUX,$(COMPILER_HOST_OS)),LINUX) 

	
endif