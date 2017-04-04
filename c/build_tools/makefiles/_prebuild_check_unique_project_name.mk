CONFIG_PROJECT_NAME :=$(strip $(CONFIG_PROJECT_NAME))

ifeq ($(CONFIG_PROJECT_NAME),$(CURR_PROJECT_NAME))
    $(info !--- name is not unique)
    $(error )
endif
