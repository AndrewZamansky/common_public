current_dir :=$(dir $(abspath $(firstword $(MAKEFILE_LIST))))
abspath_fix =$(patsubst $(current_dir)%,%,$(abspath $(1) ))

calc_parent_dir = $(patsubst  %/,%, $(dir $(abspath $(1) )))

ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
    ifeq ($(findstring 3.81,$(MAKE_VERSION)),3.81)
        calc_parent_dir = $(patsubst  %/,%, $(dir $(call abspath_fix,$(1) )))
    endif
endif

# function : mkdir_if_not_exists
# usage    : $(call mkdir_if_not_exists, /path/to/dir)
#
# added current directory (.) to wildcard because during function
# initialization the $(1) is empty . so else-part calculated like $(shell mkdir)
# and this produce error (at least in Windows)
ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
define mkdir_if_not_exists
    $(if $(wildcard $(firstword $(1) .)),,$(shell $(MKDIR) $(subst /,\,$(1))))
endef
else
define mkdir_if_not_exists
    $(if $(wildcard $(firstword $(1) .)),,$(shell $(MKDIR) $(1)))
endef
endif

# function : exit
# usage    : $(call exit,1)
#
# shell access is used to delay execution to allow all previous printings
# to be outputed
define exit
    $(if $(1),$(shell echo dummy ) $(error stopping ...),)
endef
