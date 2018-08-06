
DEFAULT_GIT_IGNORE_FILE :=$(BUILD_TOOLS_ROOT_DIR)/default.project.gitignore
CURRENT_GIT_IGNORE_FILE :=$(APP_ROOT_DIR)/.gitignore
ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
    DEFAULT_GIT_IGNORE_FILE := $(subst /,\,$(DEFAULT_GIT_IGNORE_FILE))
    CURRENT_GIT_IGNORE_FILE := $(subst /,\,$(CURRENT_GIT_IGNORE_FILE))
endif

GIT_ROOT_DIR:=

##### test for existence of git and put its directory name in GIT_ROOT_DIR #####
SEARCHED_TOOL:=git
SEARCHED_DIR_VARIABLE:=GIT_ROOT_DIR
MANUALLY_DEFINED_DIR_VARIABLE:=REDEFINE_GIT_ROOT_DIR
ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
    TEST_FILE_IN_SEARCHED_DIR:=cmd\git.exe
else ifeq ($(findstring LINUX,$(COMPILER_HOST_OS)),LINUX)
    TEST_FILE_IN_SEARCHED_DIR:=bin/git
endif
include $(MAKEFILES_ROOT_DIR)/_include_functions/tool_existence_check.mk
####### end of tool existence test #####

        
GIT ="$(GIT_ROOT_DIR)/bin/git"


## test for validity of application git repository
GIT_DIR := $(firstword $(wildcard ./.git))
ifeq ($(findstring ./.git,$(GIT_DIR)),)      # if not found ./.git in $(GIT_DIR)
    $(info err: create git project repository. you can run following command:)
    ADVISED_CMD := $(CD) $(APP_ROOT_DIR) $(SHELL_CMD_DELIMITER)
    ADVISED_CMD += $(GIT) init $(SHELL_CMD_DELIMITER)
    ADVISED_CMD += $(CP) $(DEFAULT_GIT_IGNORE_FILE) $(CURRENT_GIT_IGNORE_FILE)
    $(info ---: $(ADVISED_CMD))
    $(call exit,1)
endif


CURR_APP_SHORT_COMMIT := $(shell $(GIT) rev-parse --short=8 HEAD)

SHELL_OUT := $(shell $(GIT) status 2>&1)
TREE_CLEAN_STR :=nothing to commit, working
ifeq ($(findstring $(TREE_CLEAN_STR),$(SHELL_OUT)),)
    $(info ---- $(PROJECT_NAME) git tree is modified)
    MODIFIED_GITS +=$(PROJECT_NAME)
endif


CURR_APP_GIT_BRANCH := $(shell $(GIT) rev-parse --abbrev-ref HEAD 2>&1)
# remove heads/ prefix if exists :
CURR_APP_GIT_BRANCH := $(patsubst heads/%,%,$(CURR_APP_GIT_BRANCH))
ifneq ($(findstring ambiguous argument 'HEAD',$(CURR_APP_GIT_BRANCH)),)
    $(info err: git repository test failed : $(APP_ROOT_DIR))
    $(info ---: git error  :   $(CURR_APP_GIT_BRANCH))
    $(info ---: maybe branch was not created after git initialization)
    $(info ---: in this case create branch by running)
    $(info ---: following command [ it will add all files in directory ] :)
    ADVISED_CMD := $(CD) $(APP_ROOT_DIR) $(SHELL_CMD_DELIMITER)
    ADVISED_CMD += $(GIT) add . $(SHELL_CMD_DELIMITER)
    ADVISED_CMD += $(GIT) commit -m "initial commit"
    $(info ---: $(ADVISED_CMD))
    $(call exit,1)
endif
ifeq ($(findstring $(PROJECT_NAME),$(CURR_APP_GIT_BRANCH)),)
    # if not found $(PROJECT_NAME) in $(CURR_APP_GIT_BRANCH)
    ALLOWED_BRANCH_NAMES := $(PROJECT_NAME) or $(PROJECT_NAME)<_vVersion>
    $(info err: git repository test failed : $(APP_ROOT_DIR))
    $(info ---: branch names must be of type $(ALLOWED_BRANCH_NAMES))
    $(info ---: but current branch name is $(CURR_APP_GIT_BRANCH))
    $(info ---: in case that this git is just created run following command  :)
    ADVISED_CMD := $(CD) $(APP_ROOT_DIR) $(SHELL_CMD_DELIMITER)
    ADVISED_CMD += $(GIT) branch -m $(PROJECT_NAME)
    $(info ---: $(ADVISED_CMD))
    $(call exit,1)
endif
## end of test for validity of application git repository


# test if current commit and branch of common_public
# is the same as required by application
CURR_GIT_REPO_DIR :=$(COMMON_PUBLIC_DIR)
CURR_GIT_COMMIT_HASH_VARIABLE :=CONFIG_COMMON_PUBLIC_GIT_COMMIT_HASH
CHECKING_COMMON_PUBLIC_GIT :=y
include $(MAKEFILES_ROOT_DIR)/_include_functions/git_prebuild_repo_check.mk


ifdef CONFIG_USE_COMMON_PRIVATE_PACKAGES
    # test if current commit and branch of common_private
    # is the same as required by application
    CURR_GIT_REPO_DIR :=$(COMMON_PRIVATE_DIR)
    CURR_GIT_COMMIT_HASH_VARIABLE :=CONFIG_COMMON_PRIVATE_GIT_COMMIT_HASH
    include $(MAKEFILES_ROOT_DIR)/_include_functions/git_prebuild_repo_check.mk
else
    $(info ---- common private packages not in use)
endif
