
$(info ------  START OF VARIABLE LIST ------- )
$(info  )

$(info COMPILER_HOST_OS := $(COMPILER_HOST_OS))
$(info SHELL := $(SHELL))
$(info CALCULATED_SHELL := $(CALCULATED_SHELL))
$(info MAKE := $(MAKE))
$(info DATE_STR := $(DATE_STR))
$(info TIME_STR := $(TIME_STR))
$(info  )

$(info APP_ROOT_DIR := $(APP_ROOT_DIR))
$(info OUTPUT_APP_NAME := $(OUTPUT_APP_NAME))
$(info PROJECT_NAME := $(PROJECT_NAME))
$(info PARENT_OF_COMMON_PUBLIC_DIR := $(PARENT_OF_COMMON_PUBLIC_DIR))
$(info COMMON_PUBLIC_DIR := $(COMMON_PUBLIC_DIR))
$(info WORKSPACE_NAME := $(WORKSPACE_NAME))
$(info DRIVERS_ROOT_DIR := $(DRIVERS_ROOT_DIR))
$(info SW_PACKAGES_ROOT_DIR := $(SW_PACKAGES_ROOT_DIR))
$(info OS_ROOT_DIR := $(OS_ROOT_DIR))
$(info EXTERNAL_SOURCE_ROOT_DIR := $(EXTERNAL_SOURCE_ROOT_DIR))
$(info COMMON_PARTITION := $(COMMON_PARTITION))
$(info APP_PARTITION := $(APP_PARTITION))

$(info  )
$(info usefull makefile calls :)
$(info  DUMMY := $$(call ADD_TO_GLOBAL_INCLUDE_PATH , your/include/path))
$(info  DUMMY := $$(call ADD_TO_GLOBAL_DEFINES , your_define=xx))
$(info  DUMMY := $$(call ADD_TO_GLOBAL_LIBRARIES , var ))
$(info  DUMMY := $$(call ADD_TO_GLOBAL_LIBRARIES_PATH , var ))


list_var :
	$(info   )
	$(info ------  END OF VARIABLE LIST ------- )
