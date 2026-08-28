; ChecklistApp NSIS Installer Script
; Requires NSIS (https://nsis.sourceforge.io/)

!include "MUI2.nsh"

; --- General ---
Name "ChecklistApp"
OutFile "ChecklistApp-Setup.exe"
InstallDir "$LOCALAPPDATA\ChecklistApp"
InstallDirRegKey HKCU "Software\ChecklistApp" "InstallDir"
RequestExecutionLevel user

; --- Version ---
VIProductVersion "1.0.0"
VIAddVersionKey "ProductName" "ChecklistApp"
VIAddVersionKey "FileDescription" "Aplicación de checklist y correcciones"
VIAddVersionKey "FileVersion" "1.0.0"
VIAddVersionKey "ProductVersion" "1.0.0"

; --- MUI Settings ---
!define MUI_ABORTWARNING
!define MUI_ICON "resources\icons\app.ico"
!define MUI_UNICON "resources\icons\app.ico"
!define MUI_WELCOMEFINISHPAGE_BITMAP "resources\icons\installer.bmp"
!define MUI_UNWELCOMEFINISHPAGE_BITMAP "resources\icons\installer.bmp"

; --- Pages ---
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "LICENSE"
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

; --- Languages ---
!insertmacro MUI_LANGUAGE "Spanish"
!insertmacro MUI_LANGUAGE "English"

; --- Installer Sections ---
Section "ChecklistApp" SecMain
    SetOutPath "$INSTDIR"
    
    ; Main executable
    File "build\release\ChecklistApp.exe"
    
    ; Qt libraries (from windeployqt)
    File /nonfatal "build\release\*.dll"
    File /nonfatal "build\release\platforms\*.dll"
    File /nonfatal "build\release\styles\*.dll"
    File /nonfatal "build\release\imageformats\*.dll"
    
    ; QML files
    SetOutPath "$INSTDIR\qml"
    File /r "build\release\qml\*.*"
    
    ; Create uninstaller
    WriteUninstaller "$INSTDIR\Uninstall.exe"
    
    ; Registry entries
    WriteRegStr HKCU "Software\ChecklistApp" "InstallDir" "$INSTDIR"
    WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\ChecklistApp" "DisplayName" "ChecklistApp"
    WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\ChecklistApp" "UninstallString" '"$INSTDIR\Uninstall.exe"'
    WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\ChecklistApp" "InstallLocation" "$INSTDIR"
    WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\ChecklistApp" "DisplayVersion" "1.0.0"
    WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\ChecklistApp" "Publisher" "ChecklistApp"
    
    ; Create Start Menu shortcut
    CreateDirectory "$SMPROGRAMS\ChecklistApp"
    CreateShortCut "$SMPROGRAMS\ChecklistApp\ChecklistApp.lnk" "$INSTDIR\ChecklistApp.exe"
    CreateShortCut "$SMPROGRAMS\ChecklistApp\Uninstall.lnk" "$INSTDIR\Uninstall.exe"
    
    ; Create Desktop shortcut
    CreateShortCut "$DESKTOP\ChecklistApp.lnk" "$INSTDIR\ChecklistApp.exe"
SectionEnd

; --- Uninstaller Section ---
Section "Uninstall"
    ; Remove files
    Delete "$INSTDIR\ChecklistApp.exe"
    Delete "$INSTDIR\*.dll"
    Delete "$INSTDIR\Uninstall.exe"
    
    RMDir /r "$INSTDIR\platforms"
    RMDir /r "$INSTDIR\styles"
    RMDir /r "$INSTDIR\imageformats"
    RMDir /r "$INSTDIR\qml"
    RMDir "$INSTDIR"
    
    ; Remove Start Menu shortcuts
    Delete "$SMPROGRAMS\ChecklistApp\ChecklistApp.lnk"
    Delete "$SMPROGRAMS\ChecklistApp\Uninstall.lnk"
    RMDir "$SMPROGRAMS\ChecklistApp"
    
    ; Remove Desktop shortcut
    Delete "$DESKTOP\ChecklistApp.lnk"
    
    ; Remove registry entries
    DeleteRegKey HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\ChecklistApp"
    DeleteRegKey HKCU "Software\ChecklistApp"
SectionEnd
