### calculate date
DATE_STR := $(subst -,$(SPACE),$(strip $(DATE)))#replace '-' with ' '
DATE_STR := $(subst /,$(SPACE),$(DATE_STR))#replace '/' with ' '

DATE_YEAR_STR :=$(word 1,$(DATE_STR))
#convert year 20XX to XX and add rest of date:
DATE_STR := $(patsubst 20%,%,$(DATE_YEAR_STR)) $(wordlist 2,3,$(DATE_STR))
DATE_STR := $(subst $(SPACE),.,$(DATE_STR))#replace ' ' with '.'

### calculating version number 
TIME_STR := $(subst :,.,$(strip $(TIME)))#replace ':' with '.'

CURR_GIT_VERSION :=$(patsubst $(PROJECT_NAME)%,%,$(CURR_GIT_BRANCH))
CURR_GIT_VERSION :=$(patsubst _%,%,$(CURR_GIT_VERSION))
ifeq ($(CURR_GIT_VERSION),) # if empty then we are on main branch     
    CURR_GIT_VERSION :=MasterV0.0
endif
MAIN_VERSION_STR := $(CURR_GIT_VERSION)
FULL_VERSION_STR := $(CURR_GIT_VERSION)r$(DATE_STR).$(TIME_STR)
