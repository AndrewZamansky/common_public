
#test that arguments are set correctly 
ifeq ($(CURR_GIT_REPO_DIR),)
    $(info err: CURR_GIT_REPO_DIR (= git directory to test) should be set)
    $(info ---: before including 'git_prebuild_repo_check.mk')
    $(call exit,1)
endif
ifeq ($(CURR_GIT_COMMIT_HASH_VARIABLE),)
    $(info err: git repository test failed : $(CURR_GIT_REPO_DIR))
    $(info ---: CURR_GIT_COMMIT_HASH_VARIABLE (= name of variable that contain)
    $(info ---: requested hash) should be set)
    $(info ---: before including 'git_prebuild_repo_check.mk')
    $(call exit,1)
endif

ifeq ("","$(filter $(CURR_GIT_REPO_DIR),$(EXTERNAL_SRC_GIT_DIRS))")
    # adding new directory to external source directories list:
    EXTERNAL_SRC_GIT_DIRS := $(EXTERNAL_SRC_GIT_DIRS) $(CURR_GIT_REPO_DIR)

    GIT_REQUESTED_COMMIT :=$($(CURR_GIT_COMMIT_HASH_VARIABLE))
    ifeq (,$(GIT_REQUESTED_COMMIT))
        $(info err: git repository test failed : $(CURR_GIT_REPO_DIR))
        $(info ---: $(CURR_GIT_COMMIT_HASH_VARIABLE) variable doesn't)
        $(info ---: exist or is empty)
        $(call exit,1)
    endif

    ifeq ("",$(GIT_REQUESTED_COMMIT))
        $(info err: git repository test failed : $(CURR_GIT_REPO_DIR))
        $(info ---: $(CURR_GIT_COMMIT_HASH_VARIABLE) = $(GIT_REQUESTED_COMMIT))
        $(info ---: $(CURR_GIT_COMMIT_HASH_VARIABLE) should contain git hash)
        $(call exit,1)
    endif

    ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
    
        GIT_PARTITION := $(firstword $(subst :, ,$(CURR_GIT_REPO_DIR))):
        SHELL_GO_TO_GIT_DIR :=cd $(CURR_GIT_REPO_DIR) & $(GIT_PARTITION) & 
    
    else ifeq ($(findstring LINUX,$(COMPILER_HOST_OS)),LINUX)

        SHELL_GO_TO_GIT_DIR :=cd $(CURR_GIT_REPO_DIR) ;

    endif

    SHELL_CMD :=$(SHELL_GO_TO_GIT_DIR) $(GIT) rev-parse --abbrev-ref HEAD
    CURR_GIT_BRANCH := $(shell $(SHELL_CMD))
    #removing heads/ if exists :
    CURR_GIT_BRANCH := $(patsubst heads/%,%,$(CURR_GIT_BRANCH))
    SEEK_BRANCH:=$(filter $(CURR_APP_GIT_BRANCH),$(CURR_GIT_BRANCH))
    ifeq ($(sort $(SEEK_BRANCH)),$(CURR_APP_GIT_BRANCH))
        BRANCH_NAME_MATCH :=y
    else
        BRANCH_NAME_MATCH :=n
    endif
    
    CURR_GIT_COMMIT := $(shell $(SHELL_GO_TO_GIT_DIR) $(GIT) rev-parse HEAD)
    ifneq ("$(CURR_GIT_COMMIT)",$(GIT_REQUESTED_COMMIT))
        SHELL_CMD :=$(SHELL_GO_TO_GIT_DIR) $(GIT) status --porcelain 2>&1
        SHELL_OUT := $(shell $(SHELL_CMD))
        ERROR_MESSAGE := M 
        # if  $(CURR_APP_GIT_BRANCH) is not in $(CURR_GIT_BRANCH) list :
        ifeq (n,$(BRANCH_NAME_MATCH))
            ifeq ($(findstring $(ERROR_MESSAGE),$(SHELL_OUT)),$(ERROR_MESSAGE))
                $(info err: git repository test failed : $(CURR_GIT_REPO_DIR))
                $(info ---: git error : commit/stash all changes to)
                $(info ---: $(CURR_GIT_REPO_DIR) before checkout another commit)
                $(info ---: tested git branch is : $(CURR_GIT_BRANCH))
                $(info ---: application git branch is : $(CURR_APP_GIT_BRANCH))
                $(call exit,1)
            endif
            ERROR_MESSAGE := D #??
            ifeq ($(findstring $(ERROR_MESSAGE),$(SHELL_OUT)),$(ERROR_MESSAGE))
                $(info err: git repository test failed : $(CURR_GIT_REPO_DIR))
                $(info ---: git error : commit/stash all changes to)
                $(info ---: $(CURR_GIT_REPO_DIR) before checkout another commit)
                $(info ---: tested git branch is : $(CURR_GIT_BRANCH))
                $(info ---: application git branch is : $(CURR_APP_GIT_BRANCH))
                $(call exit,1)
            endif
        endif

        SHELL_CMD := $(SHELL_GO_TO_GIT_DIR) $(GIT) bundle 
        SHELL_CMD += verify $(CURR_GIT_BUNDLE) 2>&1
        SHELL_OUT := $(shell $(SHELL_CMD))
        REQUIRED_COMMIT_FROM_BUNDLE :=$(word 13,$(SHELL_OUT))
        NEW_COMMIT_FROM_BUNDLE :=$(word 6,$(SHELL_OUT))
    
        ifneq ($(CURR_GIT_BUNDLE),)
            SHELL_CMD := $(SHELL_GO_TO_GIT_DIR) $(GIT) cat-file
            SHELL_CMD += commit $(REQUIRED_COMMIT_FROM_BUNDLE) 2>&1
            SHELL_OUT := $(shell $(SHELL_CMD))
            ifeq ($(findstring fatal:,$(SHELL_OUT)),fatal:)
                $(info err: git repository test failed : $(CURR_GIT_REPO_DIR))
                $(info ---: commit $(REQUIRED_COMMIT_FROM_BUNDLE) , required)
                $(info ---: by $(CURR_GIT_BUNDLE) , not found. )
                $(info ---: get git repository $(CURR_GIT_REPO_DIR))
                $(info ---: that include required commit )
                $(call exit,1)
            endif
            SHELL_CMD := $(GIT) cat-file commit $(NEW_COMMIT_FROM_BUNDLE) 2>&1
            SHELL_OUT := $(shell $(SHELL_GO_TO_GIT_DIR) $(SHELL_CMD))
            ifeq ($(findstring fatal:,$(SHELL_OUT)),fatal:)
                $(info err: git repository test failed : $(CURR_GIT_REPO_DIR))
                $(info ---: commit $(NEW_COMMIT_FROM_BUNDLE) not found)
                $(info ---: apply bundle $(CURR_GIT_BUNDLE) )
                $(info ---: you can use following command :)
                CMD_TO_RUN := $(SHELL_GO_TO_GIT_DIR) $(GIT) remote add
                CMD_TO_RUN += uCProject_bundle $(CURR_GIT_BUNDLE) --fetch
                $(info ---: $(CMD_TO_RUN))
                $(call exit,1)
            endif
        endif

        $(info info/err: git repository test failed : $(CURR_GIT_REPO_DIR))
        $(info ---: current commit   : "$(CURR_GIT_COMMIT)")
        $(info ---: requested commit : $(GIT_REQUESTED_COMMIT))
        $(info ---: checkout requested commit and)
        NEED_TO_SWITCH_BRANCH_OR_COMMIT :=y
    else ifeq ($(GIT_CHECK_BRANCH_NAME),y)
        #if  $(CURR_APP_GIT_BRANCH) is not in $(CURR_GIT_BRANCH) list
        ifeq (n,$(BRANCH_NAME_MATCH))
            # for now we are doing manual checkout as it usefull in case when
            # we try mistakenly compile non-related project 
            $(info info/err: git repository test failed : $(CURR_GIT_REPO_DIR))
            $(info ---: current branch is : $(CURR_GIT_BRANCH) )
            $(info ---: current commit is OK but current branch)
            $(info ---: should be $(CURR_APP_GIT_BRANCH) .)
            NEED_TO_SWITCH_BRANCH_OR_COMMIT :=y
        endif
    else
    endif

    ifeq ($(NEED_TO_SWITCH_BRANCH_OR_COMMIT),y)
        CMD_TO_RUN := $(SHELL_GO_TO_GIT_DIR) $(GIT) checkout
        CMD_TO_RUN += $(GIT_REQUESTED_COMMIT) -B $(CURR_APP_GIT_BRANCH)
        ifneq ($(FORCE_PROJECT_SWITCH),y)
            $(info ---: move branch $(CURR_APP_GIT_BRANCH) to requested commit)
            $(info ---: you can use following command :)
            $(info ---: $(CMD_TO_RUN))
            $(call exit,1)
        else
            CURR_GIT_BRANCH := $(shell $(CMD_TO_RUN))
        endif
    else
        $(info ---- git repository $(CURR_GIT_REPO_DIR) is synchronized)
    endif
endif
#clear arguments for next function usage
CURR_GIT_REPO_DIR:=
CURR_GIT_COMMIT_HASH_VARIABLE:=
CURR_GIT_BUNDLE:=
GIT_CHECK_BRANCH_NAME:=
