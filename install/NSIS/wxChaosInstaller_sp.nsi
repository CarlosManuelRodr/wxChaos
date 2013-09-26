; WxChaos Installer
!define PRODUCT_NAME "wxChaos"
!define PRODUCT_VERSION "1.1.0"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\wxChaos.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"

; MUI 1.67 compatible ------
!include "MUI.nsh"

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON "installer_icon.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"

!define MUI_WELCOMEFINISHPAGE_BITMAP "wizard.bmp"
!define MUI_UNWELCOMEFINISHPAGE_BITMAP "wizard.bmp"

; Welcome page
!insertmacro MUI_PAGE_WELCOME
; License page
!insertmacro MUI_PAGE_LICENSE "license.txt"
; Directory page
!insertmacro MUI_PAGE_DIRECTORY
; Instfiles page
!insertmacro MUI_PAGE_INSTFILES
; Finish page
!define MUI_FINISHPAGE_RUN "$INSTDIR\wxChaos.exe"
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; Language files
!insertmacro MUI_LANGUAGE "Spanish"

; MUI end ------

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "wxChaos-1.1.0-win32_spa.exe"
InstallDir "$PROGRAMFILES\wxChaos"
InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""
ShowInstDetails show
ShowUnInstDetails show

Section "Principal" SEC01
  SetOutPath "$INSTDIR"
  SetOverwrite try
  File "..\..\bin\win32_Unicode Setup Spanish\config.ini"
  SetOutPath "$INSTDIR\Doc"
  File "..\..\bin\win32_Unicode Setup Spanish\Doc\license.txt"
  File "..\..\bin\win32_Unicode Setup Spanish\Doc\User_Manual.pdf"
  SetOutPath "$INSTDIR"
  File "..\..\bin\win32_Unicode Setup Spanish\msvcp100.dll"
  File "..\..\bin\win32_Unicode Setup Spanish\msvcr100.dll"
  SetOutPath "$INSTDIR\Resources"
  File "..\..\bin\win32_Unicode Setup Spanish\Resources\color_opt.png"
  File "..\..\bin\win32_Unicode Setup Spanish\Resources\DiavloFont.otf"
  File "..\..\bin\win32_Unicode Setup Spanish\Resources\dpDefault.bmp"
  File "..\..\bin\win32_Unicode Setup Spanish\Resources\dpDefault.dpe"
  File "..\..\bin\win32_Unicode Setup Spanish\Resources\help.png"
  File "..\..\bin\win32_Unicode Setup Spanish\Resources\HelpImage.png"
  File "..\..\bin\win32_Unicode Setup Spanish\Resources\icon.ico"
  File "..\..\bin\win32_Unicode Setup Spanish\Resources\iconPNG.png"
  File "..\..\bin\win32_Unicode Setup Spanish\Resources\keyboard.png"
  File "..\..\bin\win32_Unicode Setup Spanish\Resources\mouse.png"
  File "..\..\bin\win32_Unicode Setup Spanish\Resources\open.png"
  File "..\..\bin\win32_Unicode Setup Spanish\Resources\play.png"
  File "..\..\bin\win32_Unicode Setup Spanish\Resources\Play.tga"
  File "..\..\bin\win32_Unicode Setup Spanish\Resources\plot.png"
  File "..\..\bin\win32_Unicode Setup Spanish\Resources\prop.png"
  File "..\..\bin\win32_Unicode Setup Spanish\Resources\Stop.tga"
  SetOutPath "$INSTDIR\Resources\Tutorials"
  File "..\..\bin\win32_Unicode Setup Spanish\Resources\Tutorials\dimTut.html"
  File "..\..\bin\win32_Unicode Setup Spanish\Resources\Tutorials\dimTut_html_69360c51.png"
  File "..\..\bin\win32_Unicode Setup Spanish\Resources\Tutorials\dimTut_html_6fa63d5c.png"
  File "..\..\bin\win32_Unicode Setup Spanish\Resources\Tutorials\dimTut_html_d39ac28.png"
  File "..\..\bin\win32_Unicode Setup Spanish\Resources\Tutorials\dpTut.html"
  File "..\..\bin\win32_Unicode Setup Spanish\Resources\Tutorials\dpTut_html_342556f1.png"
  File "..\..\bin\win32_Unicode Setup Spanish\Resources\Tutorials\dpTut_html_50448500.png"
  File "..\..\bin\win32_Unicode Setup Spanish\Resources\Tutorials\mainTut.html"
  File "..\..\bin\win32_Unicode Setup Spanish\Resources\Tutorials\mainTut_html_m4dc94d3a.png"
  SetOutPath "$INSTDIR\Resources"
  File "..\..\bin\win32_Unicode Setup Spanish\Resources\wxChaosAbout.bmp"
  SetOutPath "$INSTDIR\ScriptSamples"
  File "..\..\bin\win32_Unicode Setup Spanish\ScriptSamples\Escape_angle.ans"
  File "..\..\bin\win32_Unicode Setup Spanish\ScriptSamples\Gradient.ans"
  File "..\..\bin\win32_Unicode Setup Spanish\ScriptSamples\hello_world.ans"
  File "..\..\bin\win32_Unicode Setup Spanish\ScriptSamples\Henon.ans"
  File "..\..\bin\win32_Unicode Setup Spanish\ScriptSamples\ScriptJulia.ans"
  File "..\..\bin\win32_Unicode Setup Spanish\ScriptSamples\ScriptMandelbrot.ans"
  File "..\..\bin\win32_Unicode Setup Spanish\ScriptSamples\Escape_angle.ans"
  SetOutPath "$INSTDIR\UserScripts"
  File "..\..\bin\win32_Unicode Setup Spanish\UserScripts\Duffing.ans"
  File "..\..\bin\win32_Unicode Setup Spanish\UserScripts\Gauss.ans"
  File "..\..\bin\win32_Unicode Setup Spanish\UserScripts\Newton4.ans"
  File "..\..\bin\win32_Unicode Setup Spanish\UserScripts\NewtonCos.ans"
  File "..\..\bin\win32_Unicode Setup Spanish\UserScripts\NewtonLog.ans"
  File "..\..\bin\win32_Unicode Setup Spanish\UserScripts\NewtonPol.ans"
  File "..\..\bin\win32_Unicode Setup Spanish\UserScripts\NewtonSin.ans"
  File "..\..\bin\win32_Unicode Setup Spanish\UserScripts\NewtonTan.ans"
  File "..\..\bin\win32_Unicode Setup Spanish\UserScripts\Pawn.ans"
  File "..\..\bin\win32_Unicode Setup Spanish\UserScripts\SecanteCos.ans"
  File "..\..\bin\win32_Unicode Setup Spanish\UserScripts\SecantePol.ans"
  SetOutPath "$INSTDIR"
  File "..\..\bin\win32_Unicode Setup Spanish\wxChaos.exe"
  CreateDirectory "$SMPROGRAMS\wxChaos"
  CreateShortCut "$SMPROGRAMS\wxChaos\wxChaos.lnk" "$INSTDIR\wxChaos.exe"
  CreateShortCut "$DESKTOP\wxChaos.lnk" "$INSTDIR\wxChaos.exe"
SectionEnd

Section -AdditionalIcons
  CreateShortCut "$SMPROGRAMS\wxChaos\Uninstall.lnk" "$INSTDIR\uninst.exe"
SectionEnd

Section -Post
  WriteUninstaller "$INSTDIR\uninst.exe"
  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\wxChaos.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\wxChaos.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
SectionEnd


Function un.onUninstSuccess
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK "Uninstall complete."
FunctionEnd

Function un.onInit
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "¿Are you sure you want to uninstall $(^Name) ?" IDYES +2
  Abort
FunctionEnd

Section Uninstall
  Delete "$INSTDIR\uninst.exe"
  Delete "$INSTDIR\wxChaos.exe"
  Delete "$INSTDIR\UserScripts\SecantePol.ans"
  Delete "$INSTDIR\UserScripts\SecanteCos.ans"
  Delete "$INSTDIR\UserScripts\Pawn.ans"
  Delete "$INSTDIR\UserScripts\NewtonTan.ans"
  Delete "$INSTDIR\UserScripts\NewtonSin.ans"
  Delete "$INSTDIR\UserScripts\NewtonPol.ans"
  Delete "$INSTDIR\UserScripts\NewtonLog.ans"
  Delete "$INSTDIR\UserScripts\NewtonCos.ans"
  Delete "$INSTDIR\UserScripts\Newton4.ans"
  Delete "$INSTDIR\UserScripts\Gauss.ans"
  Delete "$INSTDIR\UserScripts\Escape_angle.ans"
  Delete "$INSTDIR\UserScripts\Duffing.ans"
  Delete "$INSTDIR\ScriptSamples\ScriptMandelbrot.ans"
  Delete "$INSTDIR\ScriptSamples\ScriptJulia.ans"
  Delete "$INSTDIR\ScriptSamples\Henon.ans"
  Delete "$INSTDIR\ScriptSamples\hello_world.ans"
  Delete "$INSTDIR\ScriptSamples\Gradient.ans"
  Delete "$INSTDIR\ScriptSamples\Escape_angle.ans"
  Delete "$INSTDIR\Resources\wxChaosAbout.bmp"
  Delete "$INSTDIR\Resources\Tutorials\mainTut_html_m4dc94d3a.png"
  Delete "$INSTDIR\Resources\Tutorials\mainTut_html_m1b921b37.jpg"
  Delete "$INSTDIR\Resources\Tutorials\mainTut_html_3570d161.png"
  Delete "$INSTDIR\Resources\Tutorials\mainTut.html"
  Delete "$INSTDIR\Resources\Tutorials\dpTut_html_50448500.png"
  Delete "$INSTDIR\Resources\Tutorials\dpTut_html_342556f1.png"
  Delete "$INSTDIR\Resources\Tutorials\dpTut.html"
  Delete "$INSTDIR\Resources\Tutorials\dimTut_html_d39ac28.png"
  Delete "$INSTDIR\Resources\Tutorials\dimTut_html_6fa63d5c.png"
  Delete "$INSTDIR\Resources\Tutorials\dimTut_html_69360c51.png"
  Delete "$INSTDIR\Resources\Tutorials\dimTut_html_342556f1.png"
  Delete "$INSTDIR\Resources\Tutorials\dimTut.html"
  Delete "$INSTDIR\Resources\Stop.tga"
  Delete "$INSTDIR\Resources\prop.png"
  Delete "$INSTDIR\Resources\plot.png"
  Delete "$INSTDIR\Resources\Play.tga"
  Delete "$INSTDIR\Resources\play.png"
  Delete "$INSTDIR\Resources\open.png"
  Delete "$INSTDIR\Resources\mouse.png"
  Delete "$INSTDIR\Resources\keyboard.png"
  Delete "$INSTDIR\Resources\iconPNG.png"
  Delete "$INSTDIR\Resources\icon.ico"
  Delete "$INSTDIR\Resources\HelpImage.png"
  Delete "$INSTDIR\Resources\help.png"
  Delete "$INSTDIR\Resources\dpDefault.dpe"
  Delete "$INSTDIR\Resources\dpDefault.bmp"
  Delete "$INSTDIR\Resources\DiavloFont.otf"
  Delete "$INSTDIR\Resources\color_opt.png"
  Delete "$INSTDIR\msvcr100.dll"
  Delete "$INSTDIR\msvcp100.dll"
  Delete "$INSTDIR\Doc\User_Manual.pdf"
  Delete "$INSTDIR\Doc\license.txt"
  Delete "$INSTDIR\config.ini"

  Delete "$SMPROGRAMS\wxChaos\Uninstall.lnk"
  Delete "$DESKTOP\wxChaos.lnk"
  Delete "$SMPROGRAMS\wxChaos\wxChaos.lnk"

  RMDir "$SMPROGRAMS\wxChaos"
  RMDir "$INSTDIR\UserScripts"
  RMDir "$INSTDIR\ScriptSamples"
  RMDir "$INSTDIR\Resources\Tutorials"
  RMDir "$INSTDIR\Resources"
  RMDir "$INSTDIR\Doc"
  RMDir "$INSTDIR"

  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
  SetAutoClose true
SectionEnd
