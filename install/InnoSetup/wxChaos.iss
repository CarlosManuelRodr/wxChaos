#ifndef StageDir
  #error StageDir must point to the staged wxChaos application directory
#endif
#ifndef VCRedistPath
  #error VCRedistPath must point to vc_redist.x64.exe
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
AppPublisher=Carlos Manuel Rodriguez y Martinez
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

[Icons]
Name: "{autoprograms}\wxChaos"; Filename: "{app}\wxChaos.exe"; WorkingDir: "{app}"
Name: "{autodesktop}\wxChaos"; Filename: "{app}\wxChaos.exe"; WorkingDir: "{app}"; Tasks: desktopicon

[Run]
Filename: "{tmp}\vc_redist.x64.exe"; Parameters: "/install /quiet /norestart"; \
    StatusMsg: "{cm:InstallingVCRedist}"; Flags: waituntilterminated; Check: VCRedistNeedsInstall
Filename: "{app}\wxChaos.exe"; Description: "{cm:LaunchProgram,wxChaos}"; WorkingDir: "{app}"; \
    Flags: nowait postinstall skipifsilent

[CustomMessages]
en.InstallingVCRedist=Installing the Microsoft Visual C++ Runtime...
es.InstallingVCRedist=Instalando Microsoft Visual C++ Runtime...

[Code]
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
