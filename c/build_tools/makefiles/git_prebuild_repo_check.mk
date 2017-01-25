
CURR_GIT_REQUESTED_COMMIT :=$($(CURR_GIT_COMMIT_HASH_VARIABLE))
ifeq (,$(CURR_GIT_REQUESTED_COMMIT))
    $(info !--- $(CURR_GIT_COMMIT_HASH_VARIABLE) variable doesn't exist or is empty)
    $(error  )
endif

ifeq ("",$(CURR_GIT_REQUESTED_COMMIT))
    $(info !--- CURR_GIT_COMMIT_HASH_VARIABLE = $(CURR_GIT_REQUESTED_COMMIT))
    $(info !--- CURR_GIT_COMMIT_HASH_VARIABLE should contain valid git hash)
    $(error  )
endif

ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)

    GIT_PARTITION := $(firstword $(subst :, ,$(CURR_GIT_REPOSITORY_DIR))):
    SHELL_GO_TO_GIT_DIR :=cd $(CURR_GIT_REPOSITORY_DIR) & $(GIT_PARTITION) & 

else ifeq ($(findstring LINUX,$(COMPILER_HOST_OS)),LINUX)

    SHELL_GO_TO_GIT_DIR :=cd $(CURR_GIT_REPOSITORY_DIR) ;

endif


CURR_GIT_COMMIT := $(shell $(SHELL_GO_TO_GIT_DIR) $(GIT) rev-parse HEAD)
ifneq ("$(CURR_GIT_COMMIT)",$(CURR_GIT_REQUESTED_COMMIT))
    SHELL_OUTPUT := $(shell $(SHELL_GO_TO_GIT_DIR) $(GIT) status --porcelain 2>&1)
    ERROR_MESSAGE := M 
    ifeq ($(findstring $(ERROR_MESSAGE),$(SHELL_OUTPUT)),$(ERROR_MESSAGE))
        $(info !--- git error : commit/stash all changes to $(CURR_GIT_REPOSITORY_DIR) before checkout another commit)
        $(error  )
    endif
    ERROR_MESSAGE := D #??
    ifeq ($(findstring $(ERROR_MESSAGE),$(SHELL_OUTPUT)),$(ERROR_MESSAGE))
        $(info !--- git error : commit/stash all changes to $(CURR_GIT_REPOSITORY_DIR) before checkout another commit)
        $(error  )
    endif

    $(info !--- git repository test failed : $(CURR_GIT_REPOSITORY_DIR))
    $(info !--- current commit   : "$(CURR_GIT_COMMIT)")
    $(info !--- requested commit : $(CURR_GIT_REQUESTED_COMMIT))
    $(info !--- checkout requested commit and move to it branch $(CURR_APPLICATION_GIT_BRANCH))
    $(info !--- you can use following command :)
    $(info !--- $(SHELL_GO_TO_GIT_DIR) $(GIT) checkout $(CURR_GIT_REQUESTED_COMMIT) -B $(CURR_APPLICATION_GIT_BRANCH))
    $(error  )
else
    CURR_GIT_BRANCH := $(shell $(SHELL_GO_TO_GIT_DIR) $(GIT) rev-parse --abbrev-ref HEAD)
    CURR_GIT_BRANCH := $(patsubst heads/%,%,$(CURR_GIT_BRANCH))#removing heads/ if exists
    ifneq ($(sort $(filter $(CURR_APPLICATION_GIT_BRANCH),$(CURR_GIT_BRANCH))),$(CURR_APPLICATION_GIT_BRANCH))#if  $(CURR_APPLICATION_GIT_BRANCH) is not in $(CURR_GIT_BRANCH) list
        #for now we are doing manual checkout
        #SHELL_OUTPUT := $(shell $(SHELL_GO_TO_GIT_DIR) git checkout $(CURR_GIT_REQUESTED_COMMIT) -B $(CURR_APPLICATION_GIT_BRANCH) 2>&1)
        $(info !--- git repository test failed : $(CURR_GIT_REPOSITORY_DIR))
        $(info !--- current branch is : $(CURR_GIT_BRANCH) )
        $(info !--- current commit is OK but current branch should be $(CURR_APPLICATION_GIT_BRANCH) )
        $(info !--- move branch $(CURR_APPLICATION_GIT_BRANCH) to requested commit)
        $(info !--- you can use following command :)
        $(info !--- $(SHELL_GO_TO_GIT_DIR) git checkout $(CURR_GIT_REQUESTED_COMMIT) -B $(CURR_APPLICATION_GIT_BRANCH))
        $(error  )
    endif
endif
$(info ---- git repository $(CURR_GIT_REPOSITORY_DIR) is synchronized)
