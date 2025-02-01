; Include Modern UI

!include "MUI2.nsh"

; Variables

Var StartMenuFolder
!define ApplicationName "Nine Morris 3D"
!define ApplicationVersion "0.4.0"

; General

Name "Nine Morris 3D"
OutFile "Nine-Morris-3D-${ApplicationVersion}-Windows.exe"
Unicode True

; Default installation folder
InstallDir "$PROGRAMFILES64\${ApplicationName}"

; Get installation folder from registry, if available (not really used right now)
InstallDirRegKey HKLM "Software\${ApplicationName}" "InstallationDirectory"

; Pages

; !insertmacro MUI_PAGE_LICENSE "${NSISDIR}\Docs\Modern UI\License.txt"  ; TODO this
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY

; Start Menu Folder Page Configuration
!define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKLM"
!define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\${ApplicationName}"
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"

!insertmacro MUI_PAGE_STARTMENU Application $StartMenuFolder

!insertmacro MUI_PAGE_INSTFILES

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

; Languages

!insertmacro MUI_LANGUAGE "English"

; Installer Sections

Section "Nine Morris 3D (required)" section_game

  SetShellVarContext all

  SectionIn 1 RO

  SetOutPath "$INSTDIR"

  File /r "assets"
  File /r "assets_engine"
  File "README.txt"
  File "nine_morris_3d.exe"
  File "nine_morris_3d_engine_*.exe"

  ; Store installation folder
  WriteRegStr HKLM "Software\${ApplicationName}" "InstallationDirectory" "$INSTDIR"

  ; Make uninstaller available from Control Panel
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${ApplicationName}" \
      "DisplayName" "${ApplicationName}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${ApplicationName}" \
      "UninstallString" "$\"$INSTDIR\Uninstall.exe$\""
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${ApplicationName}" \
      "QuietUninstallString" "$\"$INSTDIR\Uninstall.exe$\" /S"
  ; WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${ApplicationName}" \  ; TODO fill these
  ;     "DisplayIcon" "$\"...$\""
  ; WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${ApplicationName}" \
  ;     "Publisher" "Simon Mărăcine"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${ApplicationName}" \
      "DisplayVersion" "${ApplicationVersion}"

  WriteUninstaller "$INSTDIR\Uninstall.exe"

  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application

    CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
    CreateShortcut "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk" "$INSTDIR\Uninstall.exe"
    CreateShortcut "$SMPROGRAMS\$StartMenuFolder\${ApplicationName}.lnk" "$INSTDIR\nine_morris_3d.exe"

  !insertmacro MUI_STARTMENU_WRITE_END

  ExecShell "open" "$INSTDIR\README.txt"

SectionEnd

; Descriptions

; Language strings
LangString DESC_section_game ${LANG_ENGLISH} "The actual game"

; Assign language strings to sections
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${section_game} $(DESC_section_game)
!insertmacro MUI_FUNCTION_DESCRIPTION_END

; Uninstaller Section

Section "Uninstall"

  SetShellVarContext all

  Delete "$INSTDIR\nine_morris_3d_engine_*.exe"
  Delete "$INSTDIR\nine_morris_3d.exe"
  Delete "$INSTDIR\README.txt"
  RMDir /r "$INSTDIR\assets_engine"
  RMDir /r "$INSTDIR\assets"

  Delete "$INSTDIR\Uninstall.exe"

  RMDir "$INSTDIR"

  !insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuFolder

  Delete "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk"
  RMDir /r "$SMPROGRAMS\$StartMenuFolder"

  DeleteRegKey HKLM "Software\${ApplicationName}"
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${ApplicationName}"

SectionEnd
