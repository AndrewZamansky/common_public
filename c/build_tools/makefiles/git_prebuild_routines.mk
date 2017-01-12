
DEFAULT_GIT_IGNORE_FILE    :=    $(COMMON_ROOT_DIR)/build_tools/default.project.gitignore
CURRENT_GIT_IGNORE_FILE    :=    $(APP_ROOT_DIR)/.gitignore


GIT_EXISTANCE_CHECK := $(shell git 2>&1)
SYSTEM_GIT_NOT_EXISTS :=n
ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)

    DEFAULT_GIT_IGNORE_FILE := $(subst /,\,$(DEFAULT_GIT_IGNORE_FILE))
    CURRENT_GIT_IGNORE_FILE := $(subst /,\,$(CURRENT_GIT_IGNORE_FILE))

    ifneq ($(findstring 'git' is not recognized as,$(GIT_EXISTANCE_CHECK)),)
        SYSTEM_GIT_NOT_EXISTS := y
    endif

else ifeq ($(findstring LINUX,$(COMPILER_HOST_OS)),LINUX)

    ifneq ($(findstring command not found,$(GIT_EXISTANCE_CHECK)),)
        SYSTEM_GIT_NOT_EXISTS := y
    endif

endif


ifeq ($(findstring y,$(SYSTEM_GIT_NOT_EXISTS)),y)

    $(info ---- system git does not exist .  trying portable git)
    ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
        GIT_DIR =$(TOOLS_ROOT_DIR)\PortableGit
        GIT =$(GIT_DIR)\bin\git
    else ifeq ($(findstring LINUX,$(COMPILER_HOST_OS)),LINUX)
        GIT_DIR =$(TOOLS_ROOT_DIR)/linux/PortableGit    
        GIT =$(GIT_DIR)/bin/git
    endif

    ifeq ("$(wildcard $(GIT_DIR))","")
        $(info !--- portable git does not exist also)
        $(info !--- install git on your system)
        ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
            $(info !--- and make sure that system PATH contain <GIT_PATH>\Git\cmd and <GIT_PATH>\Git\bin)
        endif
        $(info !--- OR download portable git and put it to $(GIT_DIR))
        $(info !--- make sure that git-cmd.exe file is located in $(GIT_DIR)/  after unpacking   )
        $(error )
    endif

    $(info ---- portable git found)

else
    $(info ---- system git found)
    GIT =git
endif


GIT_DIR := $(firstword $(wildcard ./.git))
ifeq ($(findstring ./.git,$(GIT_DIR)),)      # if not found ./.git in $(GIT_DIR)
    $(info !--- error : create git repository of project . for example by running following command : )
    $(info !--- $(CD) $(APP_ROOT_DIR) $(SHELL_CMD_DELIMITER) git init $(SHELL_CMD_DELIMITER) $(CP) $(DEFAULT_GIT_IGNORE_FILE) $(CURRENT_GIT_IGNORE_FILE))
    $(error )
endif

CURR_APPLICATION_GIT_BRANCH := $(shell git rev-parse --abbrev-ref HEAD 2>&1)
CURR_APPLICATION_GIT_BRANCH := $(patsubst heads/%,%,$(CURR_APPLICATION_GIT_BRANCH))#removing heads/ if exists
ifneq ($(findstring ambiguous argument 'HEAD',$(CURR_APPLICATION_GIT_BRANCH)),)
    $(info !--- git repository test failed : $(APP_ROOT_DIR))
    $(info !--- git error  :   $(CURR_APPLICATION_GIT_BRANCH))
    $(info !--- maybe branch was not created after git initialization )
    $(info !--- in this case create branch by running following command [ it will add all files in directory ] :)
    $(info !--- $(CD) $(APP_ROOT_DIR) $(SHELL_CMD_DELIMITER) git add . & git commit -m "initial commit")
    $(error )
endif
ifeq ($(findstring $(PROJECT_NAME),$(CURR_APPLICATION_GIT_BRANCH)),)      # if not found $(PROJECT_NAME) in $(CURR_APPLICATION_GIT_BRANCH)
    $(info !--- git repository test failed : $(APP_ROOT_DIR))
    $(info !--- error : branch names must be of type $(PROJECT_NAME) or $(PROJECT_NAME)<_vVersion>)
    $(info !--- but current branch name is $(CURR_APPLICATION_GIT_BRANCH))
    $(info !--- in case that this git is just created run following comand  :)
    $(info !--- $(CD) $(APP_ROOT_DIR) $(SHELL_CMD_DELIMITER) git branch -m $(PROJECT_NAME))
    $(error )
endif



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
