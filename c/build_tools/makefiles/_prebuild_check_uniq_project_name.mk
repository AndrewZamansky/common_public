PROJECT_NAME :=$(strip $(PROJECT_NAME))

ifeq ($(PROJECT_NAME),$(CURR_PROJECT_NAME))
    $(error PROJECT_NAME contain name that is already found as project name in other project)
endif

