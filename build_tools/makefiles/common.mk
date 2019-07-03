#################################################################
### SHELL variable cannot be exported so we have to assign it everytime here
###

SHELL := $(CALCULATED_SHELL)

######################################

EMPTY:=
SPACE:= $(EMPTY) $(EMPTY)

include $(MAKEFILES_ROOT_DIR)/_functions/update_global_variables_functions.mk

include $(MAKEFILES_ROOT_DIR)/_functions/usefull_functions.mk

.SUFFIXES:
