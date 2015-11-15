
#################################################################
### SHELL variable cannot be exported so we have to calculate it everytime in common.mk 
### but becouse this file is included before common.mk , we calclulate SHELL variable here also

###########  checking host os #############
#test if we are running in windows
SHELL := cmd
OS_TEST	:= $(shell ver)
ifeq ($(findstring Windows,$(OS_TEST)),Windows) 	 
	COMPILER_HOST_OS := WINDOWS
else # we are not in windows now

#test if we are running in linux
SHELL := sh
OS_TEST	:= $(shell uname)
ifeq ($(findstring Linux,$(OS_TEST)),Linux)
	COMPILER_HOST_OS := LINUX
else # we are not in linux now
$(error ---- UNKNOWN HOST OS ----)
endif 
	
endif

CALCULATED_SHELL :=$(SHELL)


### end calculating SHELL variable and OS
#####################################################
