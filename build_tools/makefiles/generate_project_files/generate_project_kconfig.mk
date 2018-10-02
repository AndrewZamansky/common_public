
include $(MAKEFILES_ROOT_DIR)/common.mk

include $(GENERATE_PRJ_FILES_DIR)/find_all_components.mk

KCONFIG_DIRS :=
include $(ALL_FOUND_COMPONENTS)

ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
    ECHO_EMPTY_LINE=echo.
else
    ECHO_EMPTY_LINE=echo 
endif


# add header
NEW_LINE :=mainmenu "$(PROJECT_NAME) Configuration"
FILE_CONTENT := echo $(NEW_LINE)>$(PROJECT_KCONFIG) $(SHELL_CMD_DELIMITER)

add_kconfig_string= echo config $(1)>>$(PROJECT_KCONFIG) $(SHELL_CMD_DELIMITER)\
        echo string>>$(PROJECT_KCONFIG) $(SHELL_CMD_DELIMITER)\
        echo default "$(2)">>$(PROJECT_KCONFIG) $(SHELL_CMD_DELIMITER)\
        $(ECHO_EMPTY_LINE)>>$(PROJECT_KCONFIG) $(SHELL_CMD_DELIMITER)

# add common_public start Kconfig
FILE_CONTENT += $(call add_kconfig_string,\
              KCONFIG_PUBLIC_START_DIR,$(KCONFIG_PUBLIC_START_DIR))

# add common_private start Kconfig
FILE_CONTENT += $(call add_kconfig_string,\
              KCONFIG_PRIVATE_START_DIR,$(KCONFIG_PRIVATE_START_DIR))


KCONFIG_DIRS :=$(sort $(KCONFIG_DIRS))

# convert all kconfig pathes to variables in content of "$project/kconfig"
tmp1_func =  $(call add_kconfig_string,\
              $(wordlist 1, 1, $(1)),$(wordlist 2, 2, $(1)))
tmp2_func = $(call tmp1_func,$(subst =, ,$(1))) # split 'f=path' to 'f path'
_KCONFIG_DIRS := $(foreach KCONFIG,$(KCONFIG_DIRS),$(call tmp2_func,$(KCONFIG)))
FILE_CONTENT := $(FILE_CONTENT) $(_KCONFIG_DIRS)

ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
    FILE_CONTENT := $(subst \,/,$(FILE_CONTENT))
    START_POINT :=source "$$KCONFIG_PUBLIC_START_DIR/Kconfig"
else
    FILE_CONTENT := $(subst ",\",$(FILE_CONTENT))
    START_POINT :=source '\"$$KCONFIG_PUBLIC_START_DIR/Kconfig\"'
endif
# add start point
FILE_CONTENT += echo $(START_POINT)>>$(PROJECT_KCONFIG) $(SHELL_CMD_DELIMITER)

DUMMY:=$(shell $(FILE_CONTENT))

all :
	$(info ---- auto generated Kconfig created)
