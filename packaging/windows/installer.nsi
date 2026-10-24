; NSIS-скрипт установщика YaMusic для Windows x64.
;
; Собирается в GitHub Actions (см. .github/workflows/build-windows.yml) командой:
;   makensis /DVERSION=<version> /DINSTALL_DIR=<dist> /DOUTFILE=<exe> packaging/windows/installer.nsi
;
; Входные данные:
;   VERSION    — версия приложения (например 0.10.2)
;   INSTALL_DIR — каталог, куда cmake --install положил приложение (dist)
;   OUTFILE    — путь к собираемому установщику (по умолчанию рядом со скриптом)

Unicode true

!ifndef VERSION
  !define VERSION "0.0.0"
!endif

!ifndef INSTALL_DIR
  !error "INSTALL_DIR must be defined (path to installed dist)"
!endif

!ifndef OUTFILE
  !define OUTFILE "YaMusic-${VERSION}-windows-x64.exe"
!endif

Name "YaMusic ${VERSION}"
OutFile "${OUTFILE}"
InstallDir "$PROGRAMFILES64\YaMusic"

RequestExecutionLevel admin
SetCompressor /SOLID lzma

!include "MUI2.nsh"

; ---------------------------------------------------------------
; Pages
; ---------------------------------------------------------------

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

; ---------------------------------------------------------------
; Languages
; ---------------------------------------------------------------

!insertmacro MUI_LANGUAGE "Russian"
!insertmacro MUI_LANGUAGE "English"

; ---------------------------------------------------------------
; Sections
; ---------------------------------------------------------------

Section "YaMusic" SecMain

  SetOutPath "$INSTDIR"
  File /r "${INSTALL_DIR}\*.*"

  ; exe лежит в bin рядом с qt.conf, поэтому ярлыки указывают туда.
  !if /FileExists "${INSTALL_DIR}\bin\YaMusic.exe"
  !else
    !error "YaMusic.exe not found in ${INSTALL_DIR}\bin"
  !endif

  WriteUninstaller "$INSTDIR\Uninstall.exe"

  ; Desktop and Start menu shortcuts
  CreateShortcut "$DESKTOP\YaMusic.lnk" "$INSTDIR\bin\YaMusic.exe"
  CreateDirectory "$SMPROGRAMS\YaMusic"
  CreateShortcut "$SMPROGRAMS\YaMusic\YaMusic.lnk" "$INSTDIR\bin\YaMusic.exe"
  CreateShortcut "$SMPROGRAMS\YaMusic\Uninstall.lnk" "$INSTDIR\Uninstall.exe"

  ; Uninstaller registry
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\YaMusic" \
    "DisplayName" "YaMusic ${VERSION}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\YaMusic" \
    "DisplayVersion" "${VERSION}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\YaMusic" \
    "Publisher" "andfriden"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\YaMusic" \
    "UninstallString" "$\"$INSTDIR\Uninstall.exe$\""
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\YaMusic" \
    "InstallLocation" "$INSTDIR"
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\YaMusic" \
    "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\YaMusic" \
    "NoRepair" 1

SectionEnd

; ---------------------------------------------------------------
; Uninstaller
; ---------------------------------------------------------------

Section "Uninstall"

  Delete "$DESKTOP\YaMusic.lnk"
  Delete "$SMPROGRAMS\YaMusic\YaMusic.lnk"
  Delete "$SMPROGRAMS\YaMusic\Uninstall.lnk"
  RMDir "$SMPROGRAMS\YaMusic"

  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\YaMusic"

  RMDir /r "$INSTDIR"

SectionEnd