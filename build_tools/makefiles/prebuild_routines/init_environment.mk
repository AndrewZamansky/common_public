
#################################################################
### SHELL variable cannot be exported so we have to calculate it 
### everytime in common.mk. but becouse this file is included before
### common.mk , we calclulate SHELL variable here also

###########  checking host os #############
$(info detecting host OS ... )
$(info trying Windows ... )
SHELL := cmd
OS_TEST	:= $(shell ver)
ifeq ($(findstring Windows,$(OS_TEST)),Windows) 	 
	COMPILER_HOST_OS := WINDOWS
    $(info Windows OS detected . )
    include $(PREBUILD_ROUTINES_DIR)/init_windows_environment.mk
else # we are not in windows now
    $(info Windows OS not detected . )
    $(info trying Linux ... )
    SHELL := sh
    OS_TEST	:= $(shell uname)
    ifeq ($(findstring Linux,$(OS_TEST)),Linux)
	    COMPILER_HOST_OS := LINUX
        $(info Linux OS detected . )
        include $(PREBUILD_ROUTINES_DIR)/init_linux_environment.mk
    else # we are not in linux now
        $(error ---- UNKNOWN HOST OS ----)
endif 
	
endif

CALCULATED_SHELL :=$(SHELL)


### end calculating SHELL variable and OS
#####################################################
