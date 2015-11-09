#################################################################
### SHELL variable cannot be exported so we have to assign it everytime here
###

SHELL := $(CALCULATED_SHELL)

######################################

current_dir :=$(dir $(abspath $(firstword $(MAKEFILE_LIST))))
abspath_fix =$(patsubst $(current_dir)%,%,$(abspath $(1) ))

caclulate_dir = $(patsubst  %/,%, $(dir $(abspath $(1) )))

ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS) 	 
 ifeq ($(findstring 3.81,$(MAKE_VERSION)),3.81) 	 
  caclulate_dir = $(patsubst  %/,%, $(dir $(call abspath_fix,$(1) )))
 endif
endif






