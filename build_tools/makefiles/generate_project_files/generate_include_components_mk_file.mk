
include $(MAKEFILES_ROOT_DIR)/common.mk

include $(GENERATE_PRJ_FILES_DIR)/find_all_components.mk

include $(ALL_FOUND_COMPONENTS)



######### generating $(ALL_COMPONENTS) ###############
SUBDIRS :=$(sort $(SUBDIRS))
ALL_COMPONENTS :=$(patsubst %,SUBDIRS+=%, $(SUBDIRS))

#create file with list of SUBDIRS
LIST_FILE_NAME_TRUNCATE :=$(COMPONENTS_MK)
PREFIX_FOR_EACH_ITEM :=
SUFFIX_LINE_FOR_EACH_ITEM :=
ITEMS := $(ALL_COMPONENTS)
include $(MAKEFILES_INC_FUNC_DIR)/add_item_list_to_file.mk
#end of file creation


TEMP :=$(sort $(AUTO_GLOBAL_INCLUDE_DIR))
ALL_AUTO_INCLUDE_PATH :=$(patsubst %,AUTO_GLOBAL_INCLUDE_DIRS+=%, $(TEMP))

# adding following automated include paths 
LIST_FILE_NAME_APPEND :=$(COMPONENTS_MK)
PREFIX_FOR_EACH_ITEM :=
SUFFIX_LINE_FOR_EACH_ITEM :=
ITEMS :=$(ALL_AUTO_INCLUDE_PATH)
include $(MAKEFILES_ROOT_DIR)/_include_functions/add_item_list_to_file.mk
#end of file creation
######### end of generating $(ALL_COMPONENTS) #############

all :
	$(info ---- auto generated include_components.mk created)
