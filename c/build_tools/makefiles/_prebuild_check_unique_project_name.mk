CONFIG_PROJECT_NAME :=$(strip $(CONFIG_PROJECT_NAME))

ifeq ($(CONFIG_PROJECT_NAME),$(CURR_PROJECT_NAME))
    $(info )
    $(info !--- current PROJECT_NAME=$(CURR_PROJECT_NAME) , this name is already found as project name in other project)
    $(info !--- all project names , assigned to PROJECT_NAME in .config files should be unique)
    $(error )
endif

