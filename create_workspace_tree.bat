echo OFF

if exist build_tools (
	if exist .gitmodules (
		echo ERROR:
		echo 'build_tools' folder and .gitmodules file found,
		echo looks that file is run from 'common_public' folder.
		echo Run this file in new location, not in 'uCWorkspace' or its sub-folder.
		exit /B
	)
)

if exist common_public (
	echo ERROR:
	echo 'common_public' folder found, looks that file is run from 'uCProjects' folder.
	echo Run this file in new location, not in 'uCWorkspace' or its sub-folder.
	exit /B
)

if exist uCProjects (
	echo ERROR:
	echo 'uCProjects' folder found, looks that file is run from 'uCWorkspace' folder.
	echo Run this file in new location, not in 'uCWorkspace' or its sub-folder.
	exit /B
)

mkdir uCWorkspace
mkdir uCWorkspace\external_source
mkdir uCWorkspace\ide_workspaces
mkdir uCWorkspace\ide_workspaces\linux
mkdir uCWorkspace\ide_workspaces\linux\eclipse
mkdir uCWorkspace\ide_workspaces\windows
mkdir uCWorkspace\ide_workspaces\windows\eclipse
mkdir uCWorkspace\ide_workspaces\windows\uVision
mkdir uCWorkspace\ide_workspaces\windows\visual_studio
mkdir uCWorkspace\tools
mkdir uCWorkspace\tools\linux
mkdir uCWorkspace\tools\windows
mkdir uCWorkspace\uCProjects
mkdir uCWorkspace\uCProjects\apps

SET workspace_config_file=uCWorkspace\uCProjects\workspace_config.mk

echo #>%workspace_config_file%
echo #   you can use $(PARENT_OF_COMMON_PUBLIC_DIR)  variable to define path relative to current uCProject directory>>%workspace_config_file%
echo #>>%workspace_config_file%
echo.>>%workspace_config_file%
echo.>>%workspace_config_file%

echo #REDEFINE_KCONFIG_DIR			=	$(PARENT_OF_COMMON_PUBLIC_DIR)/../tools/windows/kconfig/kconfig3.12.0>>%workspace_config_file%
echo #REDEFINE_MAKE_PROGRAM_DIR		=	$(PARENT_OF_COMMON_PUBLIC_DIR)/../tools/windows/make/make4.1>>%workspace_config_file%
echo #REDEFINE_VISUAL_STUDIO_DIR		=   C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC>>%workspace_config_file%

