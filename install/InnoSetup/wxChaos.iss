#ifndef StageDir
  #error StageDir must point to the staged wxChaos application directory
#endif
#ifndef VCRedistPath
  #error VCRedistPath must point to vc_redist.x64.exe
#endif
#ifndef WebView2BootstrapperPath
  #error WebView2BootstrapperPath must point to MicrosoftEdgeWebview2Setup.exe
#endif
#ifndef OutputDir
  #define OutputDir "Output"
#endif

#define AppExe AddBackslash(StageDir) + "wxChaos.exe"
#ifnexist AppExe
  #error The staged wxChaos.exe was not found
#endif
#ifnexist VCRedistPath
  #error The Visual C++ x64 Redistributable was not found
#endif
#ifnexist WebView2BootstrapperPath
  #error The Microsoft Edge WebView2 Runtime bootstrapper was not found
#endif

#define AppVersion GetFileProductVersionString(AppExe)
#define VCRedistMajor
#define VCRedistMinor
#define VCRedistRevision
#define VCRedistBuild
#expr GetVersionComponents(VCRedistPath, VCRedistMajor, VCRedistMinor, VCRedistRevision, VCRedistBuild)
#if AppVersion == ""
  #error wxChaos.exe does not contain product-version metadata
#endif

[Setup]
AppId={{9026D282-4438-48BF-ABD4-26CE305ADAB9}
AppName=wxChaos
AppVersion={#AppVersion}
AppVerName=wxChaos {#AppVersion}
AppPublisher=Carlos Manuel Rodriguez Martinez
AppCopyright=Copyright (C) 2012-2026 Carlos Manuel Rodriguez y Martinez
DefaultDirName={autopf}\wxChaos
DefaultGroupName=wxChaos
DisableProgramGroupPage=yes
LicenseFile=..\..\License
SetupIconFile=installer_icon.ico
UninstallDisplayIcon={app}\wxChaos.exe
OutputDir={#OutputDir}
OutputBaseFilename=wxChaos-{#AppVersion}-windows-x64-setup
Compression=lzma2/max
SolidCompression=yes
WizardStyle=modern
PrivilegesRequired=admin
PrivilegesRequiredOverridesAllowed=commandline
SetupArchitecture=x64
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
CloseApplications=yes
RestartApplications=no
UsePreviousAppDir=yes
UsePreviousLanguage=yes
UsePreviousTasks=yes
VersionInfoVersion={#AppVersion}.0
VersionInfoCompany=Carlos Manuel Rodriguez y Martinez
VersionInfoDescription=wxChaos installer
VersionInfoProductName=wxChaos
VersionInfoProductVersion={#AppVersion}

[Languages]
Name: "en"; MessagesFile: "compiler:Default.isl"
Name: "es"; MessagesFile: "compiler:Languages\Spanish.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "{#StageDir}\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#VCRedistPath}"; DestDir: "{tmp}"; DestName: "vc_redist.x64.exe"; Flags: ignoreversion; \
    Check: VCRedistNeedsInstall
Source: "{#WebView2BootstrapperPath}"; DestDir: "{tmp}"; DestName: "MicrosoftEdgeWebview2Setup.exe"; \
    Flags: ignoreversion; Check: WebView2NeedsInstall

[Icons]
Name: "{autoprograms}\wxChaos"; Filename: "{app}\wxChaos.exe"; WorkingDir: "{app}"
Name: "{autodesktop}\wxChaos"; Filename: "{app}\wxChaos.exe"; WorkingDir: "{app}"; Tasks: desktopicon

[Run]
Filename: "{tmp}\vc_redist.x64.exe"; Parameters: "/install /quiet /norestart"; \
    StatusMsg: "{cm:InstallingVCRedist}"; Flags: waituntilterminated; Check: VCRedistNeedsInstall
Filename: "{tmp}\MicrosoftEdgeWebview2Setup.exe"; Parameters: "/silent /install"; \
    StatusMsg: "{cm:InstallingWebView2}"; Flags: waituntilterminated; Check: WebView2NeedsInstall
Filename: "{app}\wxChaos.exe"; Description: "{cm:LaunchProgram,wxChaos}"; WorkingDir: "{app}"; \
    Flags: nowait postinstall skipifsilent

[CustomMessages]
en.InstallingVCRedist=Installing the Microsoft Visual C++ Runtime...
es.InstallingVCRedist=Instalando Microsoft Visual C++ Runtime...
en.InstallingWebView2=Installing the Microsoft Edge WebView2 Runtime...
es.InstallingWebView2=Instalando Microsoft Edge WebView2 Runtime...

[Code]
const
  WebView2ClientKey =
    'Software\Microsoft\EdgeUpdate\Clients\{F3017226-FE2A-4295-8BDF-00C3A9A7E4C5}';

function WebView2VersionIsInstalled(RootKey: Integer): Boolean;
var
  Version: String;
begin
  Result :=
    RegQueryStringValue(RootKey, WebView2ClientKey, 'pv', Version) and
    (Trim(Version) <> '') and
    (CompareText(Trim(Version), '0.0.0.0') <> 0);
end;

function WebView2NeedsInstall: Boolean;
begin
  { Microsoft registers per-machine WebView2 in the 32-bit registry view. }
  Result :=
    not WebView2VersionIsInstalled(HKLM32) and
    not WebView2VersionIsInstalled(HKCU);
end;

function VCRedistNeedsInstall: Boolean;
var
  Installed: Cardinal;
  Major: Cardinal;
  Minor: Cardinal;
  Build: Cardinal;
  Revision: Cardinal;
  InstalledVersion: Int64;
  RequiredVersion: Int64;
begin
  Result :=
    not RegQueryDWordValue(HKLM64, 'SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\x64',
      'Installed', Installed) or
    not RegQueryDWordValue(HKLM64, 'SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\x64',
      'Major', Major) or
    not RegQueryDWordValue(HKLM64, 'SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\x64',
      'Minor', Minor) or
    not RegQueryDWordValue(HKLM64, 'SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\x64',
      'Bld', Build) or
    not RegQueryDWordValue(HKLM64, 'SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\x64',
      'Rbld', Revision);
  if Result or (Installed <> 1) then
  begin
    Result := True;
    Exit;
  end;

  InstalledVersion := PackVersionComponents(Major, Minor, Build, Revision);
  RequiredVersion := PackVersionComponents(
    {#VCRedistMajor}, {#VCRedistMinor}, {#VCRedistRevision}, {#VCRedistBuild});
  Result := ComparePackedVersion(InstalledVersion, RequiredVersion) < 0;
end;
