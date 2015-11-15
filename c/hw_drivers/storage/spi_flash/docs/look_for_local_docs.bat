@set current_path=%~dp0
::@echo %current_path%

@set look_for_docs_makefile=%current_path%/../../../../../c/build_tools/makefiles/look_for_docs.mk
@if exist %look_for_docs_makefile% make -f %look_for_docs_makefile% WORKING_DIR=%current_path%

@set look_for_docs_makefile=%current_path%/../../../../../../c/build_tools/makefiles/look_for_docs.mk
@if exist %look_for_docs_makefile% make -f %look_for_docs_makefile% WORKING_DIR=%current_path%

@set look_for_docs_makefile=%current_path%/../../../../../../../c/build_tools/makefiles/look_for_docs.mk
@if exist %look_for_docs_makefile% make -f %look_for_docs_makefile% WORKING_DIR=%current_path%

@set look_for_docs_makefile=%current_path%/../../../../../../../../c/build_tools/makefiles/look_for_docs.mk
@if exist %look_for_docs_makefile% make -f %look_for_docs_makefile% WORKING_DIR=%current_path%

::@pause