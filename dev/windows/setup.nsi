Name "WatchmanAlert - Firehouse Alerting"
OutFile setup.exe

# Defines
!define REGKEY "SOFTWARE\$(^Name)"
!define VERSION 1.0.0
!define COMPANY "Firehouse Automation"
!define URL "http://www.firehouseautomation.com"
!define TEMP1 $R0 ;Temp variable

# MUI defines
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\modern-install-blue-full.ico"
!define MUI_FINISHPAGE_NOAUTOCLOSE
!define MUI_STARTMENUPAGE_REGISTRY_ROOT HKLM
!define MUI_STARTMENUPAGE_NODISABLE
!define MUI_STARTMENUPAGE_REGISTRY_KEY ${REGKEY}
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME StartMenuGroup
!define MUI_STARTMENUPAGE_DEFAULTFOLDER "Watchman Alerting System"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall-blue-full.ico"
!define MUI_UNFINISHPAGE_NOAUTOCLOSE
!define MUI_ABORTWARNING

!include Sections.nsh
!include MUI2.nsh
!include LogicLib.nsh
!include WinMessages.NSH
!include StrFunc.nsh

!include nsDialogs.nsh
!insertmacro NSD_FUNCTION_INIFILE

# Start registry path manipulation macro
!ifndef ENVVARUPDATE_FUNCTION
!define ENVVARUPDATE_FUNCTION
!verbose push
!verbose 3

!macro _IncludeStrFunction StrFuncName
  !ifndef ${StrFuncName}_INCLUDED
    ${${StrFuncName}}
  !endif
  !ifndef Un${StrFuncName}_INCLUDED
    ${Un${StrFuncName}}
  !endif
  !define un.${StrFuncName} "${Un${StrFuncName}}"
!macroend

!insertmacro _IncludeStrFunction StrTok
!insertmacro _IncludeStrFunction StrStr
!insertmacro _IncludeStrFunction StrRep

!macro _EnvVarUpdateConstructor ResultVar EnvVarName Action Regloc PathString
  Push "${EnvVarName}"
  Push "${Action}"
  Push "${RegLoc}"
  Push "${PathString}"
    Call EnvVarUpdate
  Pop "${ResultVar}"
!macroend
!define EnvVarUpdate '!insertmacro "_EnvVarUpdateConstructor"'

!macro _unEnvVarUpdateConstructor ResultVar EnvVarName Action Regloc PathString
  Push "${EnvVarName}"
  Push "${Action}"
  Push "${RegLoc}"
  Push "${PathString}"
    Call un.EnvVarUpdate
  Pop "${ResultVar}"
!macroend
!define un.EnvVarUpdate '!insertmacro "_unEnvVarUpdateConstructor"'

!define hklm_all_users     'HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"'
!define hkcu_current_user  'HKCU "Environment"'
# End registry path manipulation macro

Var StartMenuGroup
Var WinPCap

# Installer pages
!insertmacro MUI_PAGE_WELCOME
#!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_STARTMENU Application $StartMenuGroup
Page custom getSettings validateSettings ": Settings & Options"
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH
# Uninstaller pages
!insertmacro MUI_UNPAGE_WELCOME
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH

# Installer languages
!insertmacro MUI_LANGUAGE "English"

# Installer attributes
InstallDir "$PROGRAMFILES\Watchman"
CRCCheck on
XPStyle on
ShowInstDetails show
VIProductVersion 1.0.0.0
VIAddVersionKey ProductName "WatchmanAlert - Firehouse Alerting System"
VIAddVersionKey ProductVersion "${VERSION}"
VIAddVersionKey CompanyName "${COMPANY}"
VIAddVersionKey FileVersion "${VERSION}"
VIAddVersionKey FileDescription ""
VIAddVersionKey LegalCopyright ""
InstallDirRegKey HKLM "${REGKEY}" Path
RequestExecutionLevel user
ShowInstDetails show
ShowUninstDetails show

ReserveFile "${NSISDIR}\Plugins\InstallOptions.dll"
ReserveFile "setup.nsi.ini"

# Main installation section
Section "WatchmanAlert - Firehouse Alerting System" SectionBase

    # Recursively create required directories
    DetailPrint "Creating required directories"
    CreateDirectory $INSTDIR\audio
	CreateDirectory $INSTDIR\network

    # Install the sqlite database and tables
    DetailPrint "Installing database files"
    StrCpy $R1 ""
    SetOutPath $INSTDIR
    File /r sqlite3\sqlite3.exe
    File /r sqlite3\schema.sql

    # HTTPD files
    SetOutPath $INSTDIR\httpd
    File /r setprefs.pl
    SetOutPath $INSTDIR\httpd\www
    File /r index.htm

	SetOutPath $SYSDIR
	File /r bbxml\bin\exe\xsltproc.exe
	File /r bbxml\bin\dll\iconv.dll
	File /r bbxml\bin\dll\libexslt.dll
	File /r bbxml\bin\dll\libxml2.dll
	File /r bbxml\bin\dll\libxslt.dll
	File /r bbxml\bin\dll\zlib1.dll

    SetOutPath $INSTDIR\xml
    File /r bbxml\xml\alphasign.xsl
    File /r bbxml\xml\prolite.xsl

    # Main files
    SetOutPath $INSTDIR
    File icon1.ico
    File Watchman.exe
    File Watchman.exe
    File Watchman_Helper.exe
    File Serial_Reset.exe
    File incident.txt

    ${EnvVarUpdate} $0 "WATCHMAN_HOME" "A" "HKLM" "$INSTDIR"

    # Make it run at startup
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Run" "$(^Name)" "$INSTDIR\Watchman.exe"

    StrCmp $WinPCap "" +1 +6
    SetOutPath $TEMP
    File /r winpcap\WinPcap_4_0_2.exe
    DetailPrint "Preparing WinPCap Installation"
    sleep 500
    ExecWait "$TEMP\WinPcap_4_0_2.exe"

SectionEnd

Section -post SEC0001
    SetOutPath $INSTDIR
    WriteUninstaller $INSTDIR\uninstall.exe
    !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    SetOutPath $SMPROGRAMS\$StartMenuGroup
    CreateShortcut "$SMPROGRAMS\$StartMenuGroup\Uninstall $(^Name).lnk" $INSTDIR\uninstall.exe
    CreateShortcut "$SMPROGRAMS\$StartMenuGroup\Start $(^Name).lnk" $INSTDIR\WatchMan.exe
    !insertmacro MUI_STARTMENU_WRITE_END
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" DisplayName "$(^Name)"
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" DisplayVersion "${VERSION}"
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" Publisher "${COMPANY}"
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" DisplayIcon $INSTDIR\uninstall.exe
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" UninstallString $INSTDIR\uninstall.exe

    # Check to see if we installed sqlite, if so make sure we uninstall it too
    StrCmp $R1 "" +2
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" Sqlite3 $SYSDIR\sqlite3.exe

    WriteRegDWORD HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" NoModify 1
    WriteRegDWORD HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" NoRepair 1
SectionEnd

Section "Uninstall"
    ReadRegStr $INSTDIR HKLM "${REGKEY}" Path
    !insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuGroup

    DetailPrint "Updating start menu"
    sleep 500
    Delete "$SMPROGRAMS\$StartMenuGroup\Uninstall $(^Name).lnk"
    Delete "$SMPROGRAMS\$StartMenuGroup\Start $(^Name).lnk"
    Delete $INSTDIR\uninstall.exe
    DeleteRegValue HKLM "${REGKEY}" StartMenuGroup
    DeleteRegKey /IfEmpty HKLM "${REGKEY}"
    RmDir $SMPROGRAMS\$StartMenuGroup

    MessageBox MB_YESNO|MB_ICONQUESTION "Would you like to remove the directory containing audio files?" IDNO +1 IDYES +5
    # Do not delete the audio files
    RmDir /r $INSTDIR\xml
    Delete $INSTDIR\icon1.ico
    Delete $INSTDIR\settings.ini
    Delete $INSTDIR\schema.sql
    # Delete everything
    RmDir /r $INSTDIR

    StrCpy $R1 ""
    ReadRegStr $R1 HKEY_LOCAL_MACHINE "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" "Sqlite3"
    StrCmp $R1 "" +4
    DetailPrint "Removing database files"
    Sleep 500
    Delete $SYSDIR\sqlite3.exe

	Delete $SYSDIR\xsltproc.exe
	Delete $SYSDIR\iconv.dll
	Delete $SYSDIR\libexslt.dll
	Delete $SYSDIR\libxml2.dll
	Delete $SYSDIR\libxslt.dll
	Delete $SYSDIR\zlib1.dll


    # Remove registry values
    DetailPrint "Updating registry keys"
    sleep 500
    DeleteRegValue HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" WATCHMAN_HOME
    DeleteRegKey HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)"

    DetailPrint "Finished"

SectionEnd

Function .onInit

    InitPluginsDir
    File /oname=$PLUGINSDIR\test.ini "setup.nsi.ini"

    # Make sure we haven't previously installed the software
    StrCpy $R0 ""
    ReadRegStr $R0 HKEY_LOCAL_MACHINE "SOFTWARE\$(^Name)" Path
    StrCmp $R0 "" ProceedInstall PromptUninstall

    PromptUninstall:
        StrCpy $R1 ""
        MessageBox MB_OKCANCEL|MB_ICONEXCLAMATION "$(^Name) has previously been installed on this computer and must be uninstalled before proceeding. Click OK to launch the uninstaller or CANCEL to abort." IDOK +1 IDCANCEL AbortInstall
        CopyFiles "$R0\uninstall.exe" "$TEMP\uninstall.exe"
        ExecWait '"$TEMP\uninstall.exe" _?=$R0' $R1
        Delete "$TEMP\uninstall.exe"

        IntCmp $R1 0 ProceedInstall +1
        MessageBox MB_OK|MB_ICONEXCLAMATION "The uninstaller encountered errors and could not uninstall the $(^Name) software. Please run the uninstaller manually by choosing 'Uninstall $(^Name)' under the Start Menu -> $(^Name)."

    AbortInstall:
        Abort

    ProceedInstall:

    StrCpy $WinPCap ""
    ReadRegStr $WinPCap HKEY_LOCAL_MACHINE "SOFTWARE\WinPcap" ""
FunctionEnd

Function getSettings

  ;Display the InstallOptions dialog
  Push ${TEMP1}

    InstallOptions::dialog "$PLUGINSDIR\test.ini"
    Pop ${TEMP1}

  Pop ${TEMP1}

FunctionEnd

Function validateSettings

  ReadINIStr ${TEMP1} "$PLUGINSDIR\test.ini" "Field 2" "State"
  StrCmp ${TEMP1} 1 done

  ReadINIStr ${TEMP1} "$PLUGINSDIR\test.ini" "Field 3" "State"
  StrCmp ${TEMP1} 1 done

  ReadINIStr ${TEMP1} "$PLUGINSDIR\test.ini" "Field 4" "State"
  StrCmp ${TEMP1} 1 done
    MessageBox MB_ICONEXCLAMATION|MB_OK "You must select at least one install option!"
    Abort

  done:

FunctionEnd


!macro EnvVarUpdate UN
Function ${UN}EnvVarUpdate

  Push $0
  Exch 4
  Exch $1
  Exch 3
  Exch $2
  Exch 2
  Exch $3
  Exch
  Exch $4
  Push $5
  Push $6
  Push $7
  Push $8
  Push $9
  Push $R0

  /* After this point:
  -------------------------
     $0 = ResultVar     (returned)
     $1 = EnvVarName    (input)
     $2 = Action        (input)
     $3 = RegLoc        (input)
     $4 = PathString    (input)
     $5 = Orig EnvVar   (read from registry)
     $6 = Len of $0     (temp)
     $7 = tempstr1      (temp)
     $8 = Entry counter (temp)
     $9 = tempstr2      (temp)
     $R0 = tempChar     (temp)  */

  ; Step 1:  Read contents of EnvVarName from RegLoc
  ;
  ; Check for empty EnvVarName
  ${If} $1 == ""
    SetErrors
    DetailPrint "ERROR: EnvVarName is blank"
    Goto EnvVarUpdate_Restore_Vars
  ${EndIf}

  ; Check for valid Action
  ${If}    $2 != "A"
  ${AndIf} $2 != "P"
  ${AndIf} $2 != "R"
    SetErrors
    DetailPrint "ERROR: Invalid Action - must be A, P, or R"
    Goto EnvVarUpdate_Restore_Vars
  ${EndIf}

  ${If} $3 == HKLM
    ReadRegStr $5 ${hklm_all_users} $1     ; Get EnvVarName from all users into $5
  ${ElseIf} $3 == HKCU
    ReadRegStr $5 ${hkcu_current_user} $1  ; Read EnvVarName from current user into $5
  ${Else}
    SetErrors
    DetailPrint 'ERROR: Action is [$3] but must be "HKLM" or HKCU"'
    Goto EnvVarUpdate_Restore_Vars
  ${EndIf}

  ; Check for empty PathString
  ${If} $4 == ""
    SetErrors
    DetailPrint "ERROR: PathString is blank"
    Goto EnvVarUpdate_Restore_Vars
  ${EndIf}

  ; Make sure we've got some work to do
  ${If} $5 == ""
  ${AndIf} $2 == "R"
    SetErrors
    DetailPrint "$1 is empty - Nothing to remove"
    Goto EnvVarUpdate_Restore_Vars
  ${EndIf}

  ; Step 2: Scrub EnvVar
  ;
  StrCpy $0 $5                             ; Copy the contents to $0
  ; Remove spaces around semicolons (NOTE: spaces before the 1st entry or
  ; after the last one are not removed here but instead in Step 3)
  ${If} $0 != ""                           ; If EnvVar is not empty ...
    ${Do}
      ${${UN}StrStr} $7 $0 " ;"
      ${If} $7 == ""
        ${ExitDo}
      ${EndIf}
      ${${UN}StrRep} $0  $0 " ;" ";"         ; Remove '<space>;'
    ${Loop}
    ${Do}
      ${${UN}StrStr} $7 $0 "; "
      ${If} $7 == ""
        ${ExitDo}
      ${EndIf}
      ${${UN}StrRep} $0  $0 "; " ";"         ; Remove ';<space>'
    ${Loop}
    ${Do}
      ${${UN}StrStr} $7 $0 ";;"
      ${If} $7 == ""
        ${ExitDo}
      ${EndIf}
      ${${UN}StrRep} $0  $0 ";;" ";"
    ${Loop}

    ; Remove a leading or trailing semicolon from EnvVar
    StrCpy  $7  $0 1 0
    ${If} $7 == ";"
      StrCpy $0  $0 "" 1                   ; Change ';<EnvVar>' to '<EnvVar>'
    ${EndIf}
    StrLen $6 $0
    IntOp $6 $6 - 1
    StrCpy $7  $0 1 $6
    ${If} $7 == ";"
     StrCpy $0  $0 $6                      ; Change ';<EnvVar>' to '<EnvVar>'
    ${EndIf}
    ; DetailPrint "Scrubbed $1: [$0]"      ; Uncomment to debug
  ${EndIf}

  /* Step 3. Remove all instances of the target path/string (even if "A" or "P")
     $6 = bool flag (1 = found and removed PathString)
     $7 = a string (e.g. path) delimited by semicolon(s)
     $8 = entry counter starting at 0
     $9 = copy of $0
     $R0 = tempChar      */

  ${If} $5 != ""                           ; If EnvVar is not empty ...
    StrCpy $9 $0
    StrCpy $0 ""
    StrCpy $8 0
    StrCpy $6 0

    ${Do}
      ${${UN}StrTok} $7 $9 ";" $8 "0"      ; $7 = next entry, $8 = entry counter

      ${If} $7 == ""                       ; If we've run out of entries,
        ${ExitDo}                          ;    were done
      ${EndIf}                             ;

      ; Remove leading and trailing spaces from this entry (critical step for Action=Remove)
      ${Do}
        StrCpy $R0  $7 1
        ${If} $R0 != " "
          ${ExitDo}
        ${EndIf}
        StrCpy $7   $7 "" 1                ;  Remove leading space
      ${Loop}
      ${Do}
        StrCpy $R0  $7 1 -1
        ${If} $R0 != " "
          ${ExitDo}
        ${EndIf}
        StrCpy $7   $7 -1                  ;  Remove trailing space
      ${Loop}
      ${If} $7 == $4                       ; If string matches, remove it by not appending it
        StrCpy $6 1                        ; Set 'found' flag
      ${ElseIf} $7 != $4                   ; If string does NOT match
      ${AndIf}  $0 == ""                   ;    and the 1st string being added to $0,
        StrCpy $0 $7                       ;    copy it to $0 without a prepended semicolon
      ${ElseIf} $7 != $4                   ; If string does NOT match
      ${AndIf}  $0 != ""                   ;    and this is NOT the 1st string to be added to $0,
        StrCpy $0 $0;$7                    ;    append path to $0 with a prepended semicolon
      ${EndIf}                             ;

      IntOp $8 $8 + 1                      ; Bump counter
    ${Loop}                                ; Check for duplicates until we run out of paths
  ${EndIf}

  ; Step 4:  Perform the requested Action
  ;
  ${If} $2 != "R"                          ; If Append or Prepend
    ${If} $6 == 1                          ; And if we found the target
      DetailPrint "Target is already present in $1. It will be removed and"
    ${EndIf}
    ${If} $0 == ""                         ; If EnvVar is (now) empty
      StrCpy $0 $4                         ;   just copy PathString to EnvVar
      ${If} $6 == 0                        ; If found flag is either 0
      ${OrIf} $6 == ""                     ; or blank (if EnvVarName is empty)
        DetailPrint "$1 was empty and has been updated with the target"
      ${EndIf}
    ${ElseIf} $2 == "A"                    ;  If Append (and EnvVar is not empty),
      StrCpy $0 $0;$4                      ;     append PathString
      ${If} $6 == 1
        DetailPrint "appended to $1"
      ${Else}
        DetailPrint "Target was appended to $1"
      ${EndIf}
    ${Else}                                ;  If Prepend (and EnvVar is not empty),
      StrCpy $0 $4;$0                      ;     prepend PathString
      ${If} $6 == 1
        DetailPrint "prepended to $1"
      ${Else}
        DetailPrint "Target was prepended to $1"
      ${EndIf}
    ${EndIf}
  ${Else}                                  ; If Action = Remove
    ${If} $6 == 1                          ;   and we found the target
      DetailPrint "Target was found and removed from $1"
    ${Else}
      DetailPrint "Target was NOT found in $1 (nothing to remove)"
    ${EndIf}
    ${If} $0 == ""
      DetailPrint "$1 is now empty"
    ${EndIf}
  ${EndIf}

  ; Step 5:  Update the registry at RegLoc with the updated EnvVar and announce the change
  ;
  ClearErrors
  ${If} $3  == HKLM
    WriteRegExpandStr ${hklm_all_users} $1 $0     ; Write it in all users section
  ${ElseIf} $3 == HKCU
    WriteRegExpandStr ${hkcu_current_user} $1 $0  ; Write it to current user section
  ${EndIf}

  IfErrors 0 +4
    MessageBox MB_OK|MB_ICONEXCLAMATION "Could not write updated $1 to $3"
    DetailPrint "Could not write updated $1 to $3"
    Goto EnvVarUpdate_Restore_Vars

  ; "Export" our change
  SendMessage ${HWND_BROADCAST} ${WM_WININICHANGE} 0 "STR:Environment" /TIMEOUT=5000

  EnvVarUpdate_Restore_Vars:
  ;
  ; Restore the user's variables and return ResultVar
  Pop $R0
  Pop $9
  Pop $8
  Pop $7
  Pop $6
  Pop $5
  Pop $4
  Pop $3
  Pop $2
  Pop $1
  Push $0  ; Push my $0 (ResultVar)
  Exch
  Pop $0   ; Restore his $0

FunctionEnd

!macroend   ; EnvVarUpdate UN
!insertmacro EnvVarUpdate ""
!insertmacro EnvVarUpdate "un."
;----------------------------------- EnvVarUpdate end----------------------------------------

!verbose pop
!endif