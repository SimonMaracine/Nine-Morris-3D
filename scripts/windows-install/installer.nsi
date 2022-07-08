;Include Modern UI

  !include "MUI2.nsh"

;General

  Name "NineMorris3D"
  OutFile "Nine-Morris-3D-v0.2.0-Windows.exe"
  Unicode True

  ;Default installation folder
  InstallDir "$PROGRAMFILES64\NineMorris3D"

  ;Get installation folder from registry if available
  InstallDirRegKey HKCU "Software\NineMorris3D" ""

;Variables

  Var StartMenuFolder

;Pages

  ;!insertmacro MUI_PAGE_LICENSE "${NSISDIR}\Docs\Modern UI\License.txt"
  !insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MUI_PAGE_DIRECTORY

  ;Start Menu Folder Page Configuration
  !define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKCU" 
  !define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\NineMorris3D" 
  !define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"

  !insertmacro MUI_PAGE_STARTMENU Application $StartMenuFolder

  !insertmacro MUI_PAGE_INSTFILES

  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES

;Languages

  !insertmacro MUI_LANGUAGE "English"

;Installer Sections

Section "Nine Morris 3D (required)" section_game

  SectionIn 1 RO

  SetOutPath "$INSTDIR"

  File /r data
  File README.txt
  File Nine-Morris-3D.exe

  ;Store installation folder
  WriteRegStr HKCU "Software\NineMorris3D" "" $INSTDIR

  WriteUninstaller "$INSTDIR\Uninstall.exe"

  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application

    CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
    CreateShortcut "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk" "$INSTDIR\Uninstall.exe"
	  CreateShortcut "$SMPROGRAMS\$StartMenuFolder\Nine-Morris-3D.lnk" "$INSTDIR\Nine-Morris-3D.exe"

  !insertmacro MUI_STARTMENU_WRITE_END

SectionEnd

;Descriptions

  ;Language strings
  LangString DESC_section_game ${LANG_ENGLISH} "The actual game"

  ;Assign language strings to sections
  !insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${section_game} $(DESC_section_game)
  !insertmacro MUI_FUNCTION_DESCRIPTION_END

;Uninstaller Section

Section "Uninstall"

  Delete "$INSTDIR\Nine-Morris-3D.exe"
  Delete "$INSTDIR\README.txt"
  RMDir /r "$INSTDIR\data"

  Delete "$INSTDIR\Uninstall.exe"

  RMDir "$INSTDIR"

  RMDir /r "$DOCUMENTS\NineMorris3D"

  RMDir /r "$APPDATA\NineMorris3D"

  !insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuFolder

  Delete "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk"
  RMDir /r "$SMPROGRAMS\$StartMenuFolder"

  DeleteRegKey /ifempty HKCU "Software\NineMorris3D"

SectionEnd
