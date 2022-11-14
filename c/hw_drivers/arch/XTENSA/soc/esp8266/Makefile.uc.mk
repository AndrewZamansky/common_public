
ifeq ($(sort $(CONFIG_ESP8266)),y)

    # !!IMPORTANT!! : should be used before any 'include' statement in makefile:
    CURR_COMPONENT_DIR :=$(call get_curr_component_dir)

    ifeq ($(sort $(CONFIG_SDK_NONOS)),y)
        ESP8266_SDK_VERSION :=2.1
        SDK_ESP8266_PATH :=$(EXTERNAL_SOURCE_ROOT_DIR)/ESP8266_NONOS_SDK
    else ifeq ($(sort $(ifdef CONFIG_SDK_RTOS)),y)
        ESP8266_SDK_VERSION :=1.5
        SDK_ESP8266_PATH :=$(EXTERNAL_SOURCE_ROOT_DIR)/ESP8266_RTOS_SDK
    else
        $(info !--- unknown SDK )
        $(error )
    endif

    ifeq ("$(wildcard $(SDK_ESP8266_PATH))","")
        $(info !--- ESP8266 BSP path $(SDK_ESP8266_PATH) does not exist)
        $(info !--- download ESP8266 SDK version $(ESP8266_SDK_VERSION) and)
        $(info !--- unpack it to $(SDK_ESP8266_PATH))
        $(info !--- make sure include and lib  folders is)
        $(info !--- located in $(SDK_ESP8266_PATH)/  after unpacking)
        $(error )
    endif

    ifeq ("","$(filter $(SDK_ESP8266_PATH),$(EXTERNAL_SRC_DIRS))")
        EXTERNAL_SRC_DIRS := $(EXTERNAL_SRC_DIRS) $(SDK_ESP8266_PATH)
    endif

    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(SDK_ESP8266_PATH)/include)

    ifeq ($(sort $(CONFIG_SDK_RTOS)),y)
        DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(SDK_ESP8266_PATH)/driver_lib/include)
        DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(SDK_ESP8266_PATH)/include/espressif)
        DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(SDK_ESP8266_PATH)/include/lwip)
        DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(SDK_ESP8266_PATH)/include/lwip/ipv4)
        DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(SDK_ESP8266_PATH)/include/lwip/ipv6)
#        DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(SDK_ESP8266_PATH)/include/nopoll)
#        DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(SDK_ESP8266_PATH)/include/spiffs)
#        DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(SDK_ESP8266_PATH)/include/ssl)
#        DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(SDK_ESP8266_PATH)/include/json)
#        DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(SDK_ESP8266_PATH)/include/openssl)
    endif

    #INCLUDE_THIS_COMPONENT := y
    #INCLUDE_THIS_FOR_H_FILES_PATH :=y

    # DUMMY :=$(call SET_DIR_WITH_POSTBUILD_MAKEFILE, $(CURR_COMPONENT_DIR))
endif

include $(COMMON_CC)
