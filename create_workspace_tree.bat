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
echo #   you can use $(WORKSPACE_ROOT_DIR)  variable to define path relative to current uCProject directory>>%workspace_config_file%
echo #>>%workspace_config_file%
echo.>>%workspace_config_file%
echo.>>%workspace_config_file%

echo #REDEFINE_KCONFIG_DIR			=	$(WORKSPACE_ROOT_DIR)/../tools/windows/kconfig/kconfig3.12.0>>%workspace_config_file%
echo #REDEFINE_MAKE_PROGRAM_DIR		=	$(WORKSPACE_ROOT_DIR)/../tools/windows/make/make4.1>>%workspace_config_file%
echo #REDEFINE_VISUAL_STUDIO_DIR		=   C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC>>%workspace_config_file%
