include $(MAKEFILES_ROOT_DIR)/common.mk

MSG :=---- Build of $(APP_ROOT_DIR) finished

ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
    ifeq ($(strip $(WORKSPACE_END_OF_BUILD_NOTIFY)),y)
        MSG_CMD :=@C:\Windows\System32\mshta.exe "javascript:var sh=new ActiveXObject( 'WScript.Shell' ); sh.Popup( '$(MSG)', 300000, 'Build!', 64 );close()"
    else
        MSG_CMD :=@echo $(MSG)
    endif
endif

build_outputs:
ifdef POST_BUILD_MAKEFILE_DIR
	@echo running application specific post build script $(POST_BUILD_MAKEFILE_DIR)/Makefile.postbuild.mk
	$(MAKE) -C $(POST_BUILD_MAKEFILE_DIR) -f Makefile.postbuild.mk build_outputs
endif
	@echo ---- Build done at $(TIME_STR) ----
	$(MSG_CMD)
