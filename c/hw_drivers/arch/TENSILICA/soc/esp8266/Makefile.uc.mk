ifdef CONFIG_ESP8266
    ifdef CONFIG_SDK_NONOS
        ESP8266_SDK_VERSION :=2.1
        SDK_ESP8266_PATH :=$(EXTERNAL_SOURCE_ROOT_DIR)/ESP8266_NONOS_SDK
    else ifdef CONFIG_SDK_RTOS
        ESP8266_SDK_VERSION :=1.5
        SDK_ESP8266_PATH :=$(EXTERNAL_SOURCE_ROOT_DIR)/ESP8266_RTOS_SDK
    else
        $(info !--- unknown SDK )
        $(error )
    endif

    ifeq ("$(wildcard $(SDK_ESP8266_PATH))","")
        $(info !--- ESP8266 BSP path $(SDK_ESP8266_PATH) dont exists )
        $(info !--- download ESP8266 SDK version $(ESP8266_SDK_VERSION) and unpack it to $(SDK_ESP8266_PATH)  )
        $(info !--- make sure include and lib  folders is located in $(SDK_ESP8266_PATH)/  after unpacking   )
        $(error )
    endif

    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(SDK_ESP8266_PATH)/include)
    ifdef CONFIG_SDK_RTOS
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

    POST_BUILD_MAKEFILE_DIR :=$(COMPONENT_PATH)
endif

include $(COMMON_CC)
