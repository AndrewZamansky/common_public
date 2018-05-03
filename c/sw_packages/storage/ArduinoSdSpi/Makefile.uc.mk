

ifeq ($(sort $(CONFIG_INCLUDE_ARDUINO_SD_SPI)),y)

    INCLUDE_THIS_COMPONENT := y

    ARDUINO_SD_SPI_PATH :=$(EXTERNAL_SOURCE_ROOT_DIR)/SdFat
    #CURR_ARDUINO_SD_SPI_COMPONENT_LOCATION := $(patsubst %/Makefile.uc.mk,%,$(realpath $(filter %ArduinoSdSpi/Makefile.uc.mk,$(MAKEFILE_LIST))))
    ifeq ("$(wildcard $(ARDUINO_SD_SPI_PATH))","")
        $(info   )
        $(info !--- arduino SdSpi path $(ARDUINO_SD_SPI_PATH) doesn't exists )
        $(info !--- get SdSpi repository from https://github.com/Bodmer/SdFat.git and put it into  $(ARDUINO_SD_SPI_PATH)  )
        $(info !--- make sure that .git directory is located in $(ARDUINO_SD_SPI_PATH)/  after unpacking   )
        $(error )
    endif

    # test if current commit and branch of uboot git is the same
    # as required by application
    CURR_GIT_REPO_DIR :=$(ARDUINO_SD_SPI_PATH)
    CURR_GIT_COMMIT_HASH_VARIABLE :=CONFIG_ARDUINO_SD_SPI_GIT_COMMIT_HASH
    include $(MAKEFILES_ROOT_DIR)/_include_functions/git_prebuild_repo_check.mk

#    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(ARDUINO_SD_SPI_PATH)/src)
#    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(CURR_ARDUINO_SD_SPI_COMPONENT_LOCATION)/src)
	DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(ARDUINO_SD_SPI_PATH)/src/SdCard)
endif


DEFINES = PLATFORM_ID
#DEFINES += SPI
#DEFINES += OUTPUT
#DEFINES += MSBFIRST
#DEFINES += SPI_MODE0
CFLAGS =



#ASMFLAGS =

INCLUDE_DIR = ./src

SRC += ArduinoSdSpi.cpp
VPATH = src

SRC += SdSpiCard.cpp
SRC += SdSpiCardEX.cpp
VPATH += | $(ARDUINO_SD_SPI_PATH)/src/SdCard

include $(COMMON_CC)
