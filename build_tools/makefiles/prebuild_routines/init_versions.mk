### calculate date
DATE_STR := $(subst -,$(SPACE),$(strip $(DATE)))#replace '-' with ' '
DATE_STR := $(subst /,$(SPACE),$(DATE_STR))#replace '/' with ' '

DATE_YEAR_STR :=$(word 1,$(DATE_STR))
#convert year 20XX to XX and add rest of date:
DATE_STR := $(patsubst 20%,%,$(DATE_YEAR_STR)) $(wordlist 2,3,$(DATE_STR))
DATE_STR := $(subst $(SPACE),.,$(DATE_STR))#replace ' ' with '.'

### calculating version number 
TIME_STR := $(subst :,.,$(strip $(TIME)))#replace ':' with '.'

CURR_GIT_VERSION :=$(patsubst $(PROJECT_NAME)%,%,$(CURR_APP_GIT_BRANCH))
MAIN_VERSION_STR :=$(patsubst _%,%,$(CURR_GIT_VERSION))
ifeq ($(MAIN_VERSION_STR),) # if empty then we are on main branch
    MAIN_VERSION_STR :=MasterV0.0
endif

ifeq ($(MODIFIED_GITS),)
    REVISION_STR := r$(CURR_APP_SHORT_COMMIT)
    REVISION_FOR_FILE_STR :=$(REVISION_STR)
else
    REVISION_STR :=r$(DATE_STR).$(TIME_STR)_DEV
    REVISION_FOR_FILE_STR :=rDEV
endif
FULL_VERSION_STR := $(CURR_APP_GIT_BRANCH)$(REVISION_STR)
