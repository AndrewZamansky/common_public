CONFIG_PROJECT_NAME :=$(strip $(CONFIG_PROJECT_NAME))

ifeq ($(CONFIG_PROJECT_NAME),$(CURR_PROJECT_NAME))
    $(info !--- PROJECT_NAME contain name that is already found as project name in other project)
    $(info !--- check .config files of current workspace)
    $(error )
endif

