
#test that arguments are set correctly 
ifeq ($(CURR_SVN_REPO_DIR),)
    $(info err: CURR_SVN_REPO_DIR (= svn directory to test) should be set)
    $(info ---: before including 'svn_prebuild_repo_check.mk')
    $(call exit,1)
endif
ifeq ($(CURR_SVN_REVISION_VARIABLE),)
    $(info err: svn repository test failed : $(CURR_SVN_REPO_DIR))
    $(info ---: CURR_SVN_REVISION_VARIABLE (= name of variable that contain)
    $(info ---: requested hash) should be set)
    $(info ---: before including 'svn_prebuild_repo_check.mk')
    $(call exit,1)
endif

ifeq ("","$(filter $(CURR_SVN_REPO_DIR),$(EXTERNAL_SRC_SVN_DIRS))")
    # adding new directory to external source directories list:
    EXTERNAL_SRC_SVN_DIRS := $(EXTERNAL_SRC_SVN_DIRS) $(CURR_SVN_REPO_DIR)

    SVN_REQUESTED_REV :=$($(CURR_SVN_REVISION_VARIABLE))
    ifeq (,$(SVN_REQUESTED_REV))
        $(info err: svn repository test failed : $(CURR_SVN_REPO_DIR))
        $(info ---: $(CURR_SVN_REVISION_VARIABLE) variable doesn't)
        $(info ---: exist or is empty)
        $(call exit,1)
    endif

    ifeq ("",$(SVN_REQUESTED_REV))
        $(info err: svn repository test failed : $(CURR_SVN_REPO_DIR))
        $(info ---: $(CURR_SVN_REVISION_VARIABLE) = $(SVN_REQUESTED_REV))
        $(info ---: $(CURR_SVN_REVISION_VARIABLE) should contain svn revision)
        $(call exit,1)
    endif

    ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
    
        GIT_PARTITION := $(firstword $(subst :, ,$(CURR_SVN_REPO_DIR))):
        SHELL_GO_TO_GIT_DIR :=cd $(CURR_SVN_REPO_DIR) & $(GIT_PARTITION) & 
        SHELL_CMD :=$(SHELL_GO_TO_GIT_DIR) $(SVN) info | findstr /B Revision
    
    else ifeq ($(findstring LINUX,$(COMPILER_HOST_OS)),LINUX)

        SHELL_GO_TO_GIT_DIR :=cd $(CURR_SVN_REPO_DIR) ;
        SHELL_CMD :=$(SHELL_GO_TO_GIT_DIR) $(SVN) info | grep Revision

    endif

    SHELL_OUT := $(shell $(SHELL_CMD))
    CURR_REVISION :=$(word 2,$(SHELL_OUT))

    ifneq ("$(CURR_REVISION)",$(SVN_REQUESTED_REV))
        $(info err: svn repository test failed : $(CURR_SVN_REPO_DIR))
        $(info ---: current revision : "$(CURR_REVISION)")
        $(info ---: requested commit : $(SVN_REQUESTED_REV))
        $(info ---: checkout requested revision)
        $(info ---: you can use following command :)
        CMD_TO_RUN := $(SHELL_GO_TO_GIT_DIR) $(SVN) up -r$(SVN_REQUESTED_REV)
        $(info ---: $(CMD_TO_RUN))
        $(call exit,1)
    endif
    $(info ---- svn repository $(CURR_SVN_REPO_DIR) is synchronized)
endif
#clear arguments for next function usage
CURR_SVN_REPO_DIR:=
CURR_SVN_REVISION_VARIABLE:=
