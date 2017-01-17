
DEFAULT_GIT_IGNORE_FILE    :=    $(COMMON_ROOT_DIR)/build_tools/default.project.gitignore
CURRENT_GIT_IGNORE_FILE    :=    $(APP_ROOT_DIR)/.gitignore

GIT_ROOT_DIR:=

####### test for existence of git and put its directory name in GIT_ROOT_DIR #####
SEARCHED_TOOL:=git
SEARCHED_DIR_VARIABLE:=GIT_ROOT_DIR
MANUALLY_DEFINED_DIR_VARIABLE:=REDEFINE_GIT_ROOT_DIR
ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
    TEST_FILE_IN_SEARCHED_DIR:=bin\git.exe
else ifeq ($(findstring LINUX,$(COMPILER_HOST_OS)),LINUX)
    TEST_FILE_IN_SEARCHED_DIR:=bin/git
endif
include $(MAKEFILE_DEFS_ROOT_DIR)/tool_existence_check.mk
####### end of tool existence test #####

        
GIT ="$(GIT_ROOT_DIR)\bin\git.exe"


## test for validity of application git repository
GIT_DIR := $(firstword $(wildcard ./.git))
ifeq ($(findstring ./.git,$(GIT_DIR)),)      # if not found ./.git in $(GIT_DIR)
    $(info !--- error : create git repository of project . for example by running following command : )
    $(info !--- $(CD) $(APP_ROOT_DIR) $(SHELL_CMD_DELIMITER) $(GIT) init $(SHELL_CMD_DELIMITER) $(CP) $(DEFAULT_GIT_IGNORE_FILE) $(CURRENT_GIT_IGNORE_FILE))
    $(error )
endif

CURR_APPLICATION_GIT_BRANCH := $(shell $(GIT) rev-parse --abbrev-ref HEAD 2>&1)
CURR_APPLICATION_GIT_BRANCH := $(patsubst heads/%,%,$(CURR_APPLICATION_GIT_BRANCH))#removing heads/ if exists
ifneq ($(findstring ambiguous argument 'HEAD',$(CURR_APPLICATION_GIT_BRANCH)),)
    $(info !--- git repository test failed : $(APP_ROOT_DIR))
    $(info !--- git error  :   $(CURR_APPLICATION_GIT_BRANCH))
    $(info !--- maybe branch was not created after git initialization )
    $(info !--- in this case create branch by running following command [ it will add all files in directory ] :)
    $(info !--- $(CD) $(APP_ROOT_DIR) $(SHELL_CMD_DELIMITER) $(GIT) add . & $(GIT) commit -m "initial commit")
    $(error )
endif
ifeq ($(findstring $(PROJECT_NAME),$(CURR_APPLICATION_GIT_BRANCH)),)      # if not found $(PROJECT_NAME) in $(CURR_APPLICATION_GIT_BRANCH)
    $(info !--- git repository test failed : $(APP_ROOT_DIR))
    $(info !--- error : branch names must be of type $(PROJECT_NAME) or $(PROJECT_NAME)<_vVersion>)
    $(info !--- but current branch name is $(CURR_APPLICATION_GIT_BRANCH))
    $(info !--- in case that this git is just created run following comand  :)
    $(info !--- $(CD) $(APP_ROOT_DIR) $(SHELL_CMD_DELIMITER) $(GIT) branch -m $(PROJECT_NAME))
    $(error )
endif
## end of test for validity of application git repository



#test if current commit and branch of common_public is the same as required by application
CURR_GIT_REPOSITORY_DIR :=$(COMMON_DIR)
CURR_GIT_COMMIT_HASH_VARIABLE :=CONFIG_COMMON_PUBLIC_GIT_COMMIT_HASH
include $(MAKEFILE_DEFS_ROOT_DIR)/git_prebuild_repo_check.mk


ifdef CONFIG_USE_COMMON_PRIVATE_PACKAGES
    #test if current commit and branch of common_private is the same as required by application
    CURR_GIT_REPOSITORY_DIR :=$(COMMON_PRIVATE_DIR)
    CURR_GIT_COMMIT_HASH_VARIABLE :=CONFIG_COMMON_PRIVATE_GIT_COMMIT_HASH
    include $(MAKEFILE_DEFS_ROOT_DIR)/git_prebuild_repo_check.mk
else
    $(info ---- common private packages not in use)
endif
