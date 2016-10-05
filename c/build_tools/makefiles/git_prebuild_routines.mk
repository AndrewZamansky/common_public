

GIT_EXISTANCE_CHECK := $(shell git 2>&1)
SYSTEM_GIT_NOT_EXISTS :=n
ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)

    ifneq ($(findstring 'git' is not recognized as,$(GIT_EXISTANCE_CHECK)),)
        SYSTEM_GIT_NOT_EXISTS := y
    endif

else ifeq ($(findstring LINUX,$(COMPILER_HOST_OS)),LINUX)

    GIT_EXISTANCE_CHECK := $(shell git 2>&1)
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
        $(info !--- install git on your system )
        $(info !--- or download portable git and put it to $(GIT_DIR))
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

CURR_GIT_BRANCH := $(shell git rev-parse --abbrev-ref HEAD 2>&1)
CURR_GIT_BRANCH := $(patsubst heads/%,%,$(CURR_GIT_BRANCH))#removing heads/ if exists
ifneq ($(findstring ambiguous argument 'HEAD',$(CURR_GIT_BRANCH)),)
    $(info !--- git error  :   $(CURR_GIT_BRANCH))
    $(info !--- maybe branch was not created after git initialization )
    $(info !--- in this case create branch you can run following comands to add all files in ):)
    $(info !--- $(CD) $(APP_ROOT_DIR) $(SHELL_CMD_DELIMITER) git add . & git commit -m "initial commit")
    $(error )
endif
ifeq ($(findstring $(PROJECT_NAME),$(CURR_GIT_BRANCH)),)      # if not found $(PROJECT_NAME) in $(CURR_GIT_BRANCH)
    $(info !--- error : branch names must be of type $(PROJECT_NAME) or $(PROJECT_NAME)<_vVersion>)
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
        $(info  !--- git error : commit all changes to common_public git($(COMMON_DIR)) before changing branch or project)
        $(info  !--- current application git branch :   $(CURR_GIT_BRANCH) )
        $(info  !--- current common_public git branch :   $(CURR_COMMON_GIT_BRANCH) )
        $(error  )
    endif
    ERROR_MESSAGE := D #??
    ifeq ($(findstring $(ERROR_MESSAGE),$(SHELL_OUTPUT)),$(ERROR_MESSAGE))
        $(info  !--- git error : commit all changes to common_public git)
        $(error  )
    endif
   #for now we are doing manual checkout
   #SHELL_OUTPUT := $(shell $(SHELL_GO_TO_COMMON_GIT_DIR) git checkout $(CURR_GIT_BRANCH) 2>&1)
    SHELL_OUTPUT := $(shell $(SHELL_GO_TO_COMMON_GIT_DIR) git branch 2>&1)
    CURR_GIT_BRANCH:=$(patsubst heads/%,%,$(CURR_GIT_BRANCH))
    ifneq ($(sort $(filter $(CURR_GIT_BRANCH),$(SHELL_OUTPUT))),$(CURR_GIT_BRANCH))#if  $(CURR_GIT_BRANCH) is in $(SHELL_OUTPUT)
        $(info !--- git error : branch $(CURR_GIT_BRANCH) not found in common_public git . create it)
        $(info !--- in case that this git is just created run following comand in common_public directory:)
        $(info !--- $(SHELL_GO_TO_COMMON_GIT_DIR) git branch $(PROJECT_NAME))
        $(error  )
    else
        $(info !--- checkout $(CURR_GIT_BRANCH) manually in common_public git)
        $(info !--- you can run following comand in common_public directory:)
        $(info !--- $(SHELL_GO_TO_COMMON_GIT_DIR) git checkout $(CURR_GIT_BRANCH))
        $(error  )
    endif
endif

CURR_COMMON_PRIVATE_GIT_BRANCH := $(shell $(SHELL_GO_TO_COMMON_PRIVATE_GIT_DIR) git rev-parse --abbrev-ref HEAD)
CURR_COMMON_PRIVATE_GIT_BRANCH := $(patsubst heads/%,%,$(CURR_COMMON_PRIVATE_GIT_BRANCH))#removing heads/ if exists
ifneq ($(sort $(filter $(CURR_GIT_BRANCH),$(CURR_COMMON_PRIVATE_GIT_BRANCH))),$(CURR_GIT_BRANCH))#if  $(CURR_GIT_BRANCH) is in $(SHELL_OUTPUT)
    SHELL_OUTPUT := $(shell $(SHELL_GO_TO_COMMON_PRIVATE_GIT_DIR) git status --porcelain 2>&1)
    ERROR_MESSAGE := M 
    ifeq ($(findstring $(ERROR_MESSAGE),$(SHELL_OUTPUT)),$(ERROR_MESSAGE))
        $(info  !--- git error : commit all changes to common_private git($(COMMON_PRIVATE_DIR)) before changing branch or project)
        $(info  !--- current application git branch :   $(CURR_GIT_BRANCH) )
        $(info  !--- current common_private git branch :   $(CURR_COMMON_PRIVATE_GIT_BRANCH) )
        $(error  )
    endif
    ERROR_MESSAGE := D #??
    ifeq ($(findstring $(ERROR_MESSAGE),$(SHELL_OUTPUT)),$(ERROR_MESSAGE))
        $(info  !--- git error : commit all changes to common_private git)
        $(error  )
    endif
   #for now we are doing manual checkout
   #SHELL_OUTPUT := $(shell $(SHELL_GO_TO_COMMON_PRIVATE_GIT_DIR) git checkout $(CURR_GIT_BRANCH) 2>&1)
    SHELL_OUTPUT := $(shell $(SHELL_GO_TO_COMMON_PRIVATE_GIT_DIR) git branch 2>&1)
    CURR_GIT_BRANCH:=$(patsubst heads/%,%,$(CURR_GIT_BRANCH))
    ifneq ($(sort $(filter $(CURR_GIT_BRANCH),$(SHELL_OUTPUT))),$(CURR_GIT_BRANCH))#if  $(CURR_GIT_BRANCH) is in $(SHELL_OUTPUT)
        $(info !--- git error : branch $(CURR_GIT_BRANCH) not found in common_private git . create it)
        $(info !--- in case that this git is just created run following comand in common_private directory:)
        $(info !--- $(SHELL_GO_TO_COMMON_PRIVATE_GIT_DIR) git branch $(PROJECT_NAME))
        $(error  )
    else
        $(info !--- checkout $(CURR_GIT_BRANCH) manually in common_private git)
        $(info !--- you can run following comand in common_private directory:)
        $(info !--- $(SHELL_GO_TO_COMMON_PRIVATE_GIT_DIR) git checkout $(CURR_GIT_BRANCH))
        $(error  )
    endif
endif
