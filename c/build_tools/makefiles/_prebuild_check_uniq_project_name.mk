CONFIG_PROJECT_NAME :=$(strip $(CONFIG_PROJECT_NAME))

ifeq ($(CONFIG_PROJECT_NAME),$(CURR_PROJECT_NAME))
    $(error PROJECT_NAME contain name that is already found as project name in other project)
endif

