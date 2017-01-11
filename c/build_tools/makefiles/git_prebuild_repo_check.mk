
ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)

    GIT_PARTITION := $(firstword $(subst :, ,$(CURR_GIT_REPOSITORY_DIR))):
    SHELL_GO_TO_GIT_DIR :=cd $(CURR_GIT_REPOSITORY_DIR) & $(GIT_PARTITION) & 
    
else ifeq ($(findstring LINUX,$(COMPILER_HOST_OS)),LINUX)

    SHELL_GO_TO_GIT_DIR :=cd $(CURR_GIT_REPOSITORY_DIR) ;
    
endif

CURR_GIT_COMMIT := $(shell $(SHELL_GO_TO_GIT_DIR) git rev-parse HEAD)
ifneq ("$(CURR_GIT_COMMIT)",$(CURR_GIT_REQUESTED_COMMIT))
    SHELL_OUTPUT := $(shell $(SHELL_GO_TO_GIT_DIR) git status --porcelain 2>&1)
    ERROR_MESSAGE := M 
    ifeq ($(findstring $(ERROR_MESSAGE),$(SHELL_OUTPUT)),$(ERROR_MESSAGE))
        $(info  !--- git error : commit all changes to $(CURR_GIT_REPOSITORY_DIR) checkout another commit)
        $(error  )
    endif
    ERROR_MESSAGE := D #??
    ifeq ($(findstring $(ERROR_MESSAGE),$(SHELL_OUTPUT)),$(ERROR_MESSAGE))
        $(info  !--- git error : commit all changes to $(CURR_GIT_REPOSITORY_DIR) checkout another commit)
        $(error  )
    endif

    $(info !--- git repository test failed : $(CURR_GIT_REPOSITORY))
    $(info !--- current commit   : "$(CURR_GIT_COMMIT)")
    $(info !--- requested commit : $(CURR_GIT_REQUESTED_COMMIT))
    $(info !--- checkout requested commit and move to it branch $(CURR_APPLICATION_GIT_BRANCH))
    $(info !--- you can use following command :)
    $(info !--- $(SHELL_GO_TO_GIT_DIR) git checkout $(CONFIG_COMMON_PUBLIC_GIT_COMMIT_HASH) -B $(CURR_APPLICATION_GIT_BRANCH))

else
    CURR_GIT_BRANCH := $(shell $(SHELL_GO_TO_GIT_DIR) git rev-parse --abbrev-ref HEAD)
    CURR_GIT_BRANCH := $(patsubst heads/%,%,$(CURR_GIT_BRANCH))#removing heads/ if exists
    ifneq ($(sort $(filter $(CURR_APPLICATION_GIT_BRANCH),$(CURR_GIT_BRANCH))),$(CURR_APPLICATION_GIT_BRANCH))#if  $(CURR_APPLICATION_GIT_BRANCH) is not in $(CURR_GIT_BRANCH) list
        SHELL_OUTPUT := $(shell $(SHELL_GO_TO_COMMON_PUBLIC_GIT_DIR) git status --porcelain 2>&1)
        ERROR_MESSAGE := M 
        ifeq ($(findstring $(ERROR_MESSAGE),$(SHELL_OUTPUT)),$(ERROR_MESSAGE))
            $(info  !--- git error : commit all changes to common_public git($(COMMON_DIR)) before changing branch or project)
            $(info  !--- current application git branch :   $(CURR_GIT_BRANCH) )
            $(info  !--- current common_public git branch :   $(CURR_COMMON_PUBLIC_GIT_BRANCH) )
            $(error  )
        endif
        ERROR_MESSAGE := D #??
        ifeq ($(findstring $(ERROR_MESSAGE),$(SHELL_OUTPUT)),$(ERROR_MESSAGE))
            $(info  !--- git error : commit all changes to common_public git)
            $(error  )
        endif

        #test if current application branch name exists in common_public
        SHELL_OUTPUT := $(shell $(SHELL_GO_TO_COMMON_PUBLIC_GIT_DIR) git branch 2>&1)
        CURR_GIT_BRANCH:=$(patsubst heads/%,%,$(CURR_GIT_BRANCH))
        ifneq ($(sort $(filter $(CURR_GIT_BRANCH),$(SHELL_OUTPUT))),$(CURR_GIT_BRANCH))#if  $(CURR_GIT_BRANCH) is in $(SHELL_OUTPUT)
            $(info !--- git error : branch $(CURR_GIT_BRANCH) not found in common_public git . create it)
            $(info !--- in case that this git is just created run following comand in common_public directory:)
            $(info !--- $(SHELL_GO_TO_COMMON_PUBLIC_GIT_DIR) git branch $(CURR_GIT_BRANCH))
            $(error  )
        else
           #for now we are doing manual checkout
           #SHELL_OUTPUT := $(shell $(SHELL_GO_TO_COMMON_PUBLIC_GIT_DIR) git checkout $(CURR_GIT_BRANCH) 2>&1)
                $(info !--- checkout $(CURR_GIT_BRANCH) manually in common_public git)
            $(info !--- you can run following comand in common_public directory:)
            $(info !--- $(SHELL_GO_TO_COMMON_PUBLIC_GIT_DIR) git checkout $(CURR_GIT_BRANCH))
            $(error  )
        endif
    endif
endif
$(error ---- )
