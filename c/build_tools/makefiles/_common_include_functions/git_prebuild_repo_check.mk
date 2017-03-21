
#test that arguments are set correctly 
ifeq ($(CURR_GIT_REPOSITORY_DIR),)
    $(info !--- CURR_GIT_REPOSITORY_DIR (= git directory to test) should be set before including this file)
    $(error )
endif
ifeq ($(CURR_GIT_COMMIT_HASH_VARIABLE),)
    $(info !--- git repository test failed : $(CURR_GIT_REPOSITORY_DIR))
    $(info !--- CURR_GIT_COMMIT_HASH_VARIABLE (= name of variable that contain requested hash) should be set before including this file)
    $(error )
endif


CURR_GIT_REQUESTED_COMMIT :=$($(CURR_GIT_COMMIT_HASH_VARIABLE))
ifeq (,$(CURR_GIT_REQUESTED_COMMIT))
    $(info !--- git repository test failed : $(CURR_GIT_REPOSITORY_DIR))
    $(info !--- $(CURR_GIT_COMMIT_HASH_VARIABLE) variable doesn't exist or is empty)
    $(error  )
endif

ifeq ("",$(CURR_GIT_REQUESTED_COMMIT))
    $(info !--- git repository test failed : $(CURR_GIT_REPOSITORY_DIR))
    $(info !--- $(CURR_GIT_COMMIT_HASH_VARIABLE) = $(CURR_GIT_REQUESTED_COMMIT))
    $(info !--- $(CURR_GIT_COMMIT_HASH_VARIABLE) should contain valid git hash)
    $(error  )
endif

ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)

    GIT_PARTITION := $(firstword $(subst :, ,$(CURR_GIT_REPOSITORY_DIR))):
    SHELL_GO_TO_GIT_DIR :=cd $(CURR_GIT_REPOSITORY_DIR) & $(GIT_PARTITION) & 

else ifeq ($(findstring LINUX,$(COMPILER_HOST_OS)),LINUX)

    SHELL_GO_TO_GIT_DIR :=cd $(CURR_GIT_REPOSITORY_DIR) ;

endif


CURR_GIT_BRANCH := $(shell $(SHELL_GO_TO_GIT_DIR) $(GIT) rev-parse --abbrev-ref HEAD)
CURR_GIT_BRANCH := $(patsubst heads/%,%,$(CURR_GIT_BRANCH))#removing heads/ if exists
CURR_GIT_COMMIT := $(shell $(SHELL_GO_TO_GIT_DIR) $(GIT) rev-parse HEAD)
ifneq ("$(CURR_GIT_COMMIT)",$(CURR_GIT_REQUESTED_COMMIT))
    SHELL_OUTPUT := $(shell $(SHELL_GO_TO_GIT_DIR) $(GIT) status --porcelain 2>&1)
    ERROR_MESSAGE := M 
    ifneq ($(sort $(filter $(CURR_APPLICATION_GIT_BRANCH),$(CURR_GIT_BRANCH))),$(CURR_APPLICATION_GIT_BRANCH))#if  $(CURR_APPLICATION_GIT_BRANCH) is not in $(CURR_GIT_BRANCH) list
        ifeq ($(findstring $(ERROR_MESSAGE),$(SHELL_OUTPUT)),$(ERROR_MESSAGE))
            $(info !--- git repository test failed : $(CURR_GIT_REPOSITORY_DIR))
            $(info !--- git error : commit/stash all changes to $(CURR_GIT_REPOSITORY_DIR) before checkout another commit)
            $(info !--- tested git branch is : $(CURR_GIT_BRANCH))
            $(info !--- application git branch is : $(CURR_APPLICATION_GIT_BRANCH))
            $(error  )
        endif
        ERROR_MESSAGE := D #??
        ifeq ($(findstring $(ERROR_MESSAGE),$(SHELL_OUTPUT)),$(ERROR_MESSAGE))
            $(info !--- git repository test failed : $(CURR_GIT_REPOSITORY_DIR))
            $(info !--- git error : commit/stash all changes to $(CURR_GIT_REPOSITORY_DIR) before checkout another commit)
            $(info !--- tested git branch is : $(CURR_GIT_BRANCH))
            $(info !--- application git branch is : $(CURR_APPLICATION_GIT_BRANCH))
            $(error  )
        endif
    endif

    SHELL_OUTPUT := $(shell $(SHELL_GO_TO_GIT_DIR) $(GIT) bundle verify $(CURR_GIT_BUNDLE) 2>&1)
    REQUIRED_COMMIT_FROM_BUNDLE :=$(word 13,$(SHELL_OUTPUT))
    NEW_COMMIT_FROM_BUNDLE :=$(word 6,$(SHELL_OUTPUT))

    ifneq ($(CURR_GIT_BUNDLE),)
        SHELL_OUTPUT := $(shell $(SHELL_GO_TO_GIT_DIR) $(GIT) cat-file commit $(REQUIRED_COMMIT_FROM_BUNDLE) 2>&1)
        ifeq ($(findstring fatal:,$(SHELL_OUTPUT)),fatal:)
            $(info !--- git repository test failed : $(CURR_GIT_REPOSITORY_DIR))
            $(info !--- commit $(REQUIRED_COMMIT_FROM_BUNDLE) , required by $(CURR_GIT_BUNDLE) , not found)
            $(info !--- get git repository $(CURR_GIT_REPOSITORY_DIR) with required commit )
            $(error  )
        endif
        SHELL_OUTPUT := $(shell $(SHELL_GO_TO_GIT_DIR) $(GIT) cat-file commit $(NEW_COMMIT_FROM_BUNDLE) 2>&1)
        ifeq ($(findstring fatal:,$(SHELL_OUTPUT)),fatal:)
            $(info !--- git repository test failed : $(CURR_GIT_REPOSITORY_DIR))
            $(info !--- commit $(NEW_COMMIT_FROM_BUNDLE) not found)
            $(info !--- apply bundle $(CURR_GIT_BUNDLE) )
            $(info !--- you can use following command :)
            $(info !--- $(SHELL_GO_TO_GIT_DIR) $(GIT) remote add uCProject_bundle $(CURR_GIT_BUNDLE) --fetch)
            $(error  )
        endif
    endif

    $(info !--- git repository test failed : $(CURR_GIT_REPOSITORY_DIR))
    $(info !--- current commit   : "$(CURR_GIT_COMMIT)")
    $(info !--- requested commit : $(CURR_GIT_REQUESTED_COMMIT))
    $(info !--- checkout requested commit and move to it branch $(CURR_APPLICATION_GIT_BRANCH))
    $(info !--- you can use following command :)
    $(info !--- $(SHELL_GO_TO_GIT_DIR) $(GIT) checkout $(CURR_GIT_REQUESTED_COMMIT) -B $(CURR_APPLICATION_GIT_BRANCH))
    $(error  )
else
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

#clear arguments for next function usage
CURR_GIT_REPOSITORY_DIR:=
CURR_GIT_COMMIT_HASH_VARIABLE:=
CURR_GIT_BUNDLE:=
