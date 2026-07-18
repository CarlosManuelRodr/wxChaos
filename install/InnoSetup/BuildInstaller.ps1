[CmdletBinding()]
param(
    [string]$BuildDirectory = "cmake-build-release",
    [string]$OutputDirectory,
    [string]$CMakePath,
    [string]$InnoCompilerPath = "C:\Program Files\Inno Setup 7\ISCC.exe",
    [string]$VisualStudioDeveloperCommand =
        "C:\Program Files\Microsoft Visual Studio\18\Community\Common7\Tools\VsDevCmd.bat",
    [string]$VCRedistPath
)

$ErrorActionPreference = "Stop"
$repositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..")).Path
$buildPath = if ([System.IO.Path]::IsPathRooted($BuildDirectory)) {
    [System.IO.Path]::GetFullPath($BuildDirectory)
} else {
    [System.IO.Path]::GetFullPath((Join-Path $repositoryRoot $BuildDirectory))
}

if (-not $OutputDirectory) {
    $OutputDirectory = Join-Path $buildPath "package\installer"
}
$outputPath = [System.IO.Path]::GetFullPath($OutputDirectory)

if (-not $CMakePath) {
    $cmakeCommand = Get-Command cmake.exe -ErrorAction SilentlyContinue
    if ($cmakeCommand) {
        $CMakePath = $cmakeCommand.Source
    } else {
        $CMakePath = Join-Path $env:LOCALAPPDATA "Programs\CLion\bin\cmake\win\x64\bin\cmake.exe"
    }
}

foreach ($requiredFile in @($CMakePath, $InnoCompilerPath, $VisualStudioDeveloperCommand)) {
    if (-not (Test-Path -LiteralPath $requiredFile -PathType Leaf)) {
        throw "Required tool was not found: $requiredFile"
    }
}
if (-not (Test-Path -LiteralPath $buildPath -PathType Container)) {
    throw "The configured build directory does not exist: $buildPath"
}

if (-not $VCRedistPath) {
    $redistRoot = "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Redist\MSVC"
    $VCRedistPath = Get-ChildItem -LiteralPath $redistRoot -Recurse -Filter "vc_redist.x64.exe" |
        Sort-Object LastWriteTime -Descending |
        Select-Object -First 1 -ExpandProperty FullName
}
if (-not $VCRedistPath -or -not (Test-Path -LiteralPath $VCRedistPath -PathType Leaf)) {
    throw "The Microsoft Visual C++ x64 Redistributable could not be found."
}

# Import the MSVC developer environment into this PowerShell process so Ninja
# can rebuild C++ sources before staging the installer payload.
$developerEnvironment = & cmd.exe /d /s /c (
    'call "' + $VisualStudioDeveloperCommand + '" -arch=x64 -host_arch=x64 >nul && set'
)
if ($LASTEXITCODE -ne 0) {
    throw "Visual Studio developer environment initialization failed."
}
foreach ($line in $developerEnvironment) {
    $separator = $line.IndexOf("=")
    if ($separator -gt 0) {
        $name = $line.Substring(0, $separator)
        $value = $line.Substring($separator + 1)
        Set-Item -LiteralPath "Env:$name" -Value $value
    }
}

& $CMakePath --build $buildPath --target wxChaosStage
if ($LASTEXITCODE -ne 0) {
    throw "The wxChaos build or staging step failed."
}

$stagePath = Join-Path $buildPath "package\wxChaos"
$stagedExecutable = Join-Path $stagePath "wxChaos.exe"
if (-not (Test-Path -LiteralPath $stagedExecutable -PathType Leaf)) {
    throw "The staged executable was not created: $stagedExecutable"
}

New-Item -ItemType Directory -Path $outputPath -Force | Out-Null
$installerScript = Join-Path $PSScriptRoot "wxChaos.iss"
$compilerArguments = @(
    "/Qp",
    "/DStageDir=$stagePath",
    "/DVCRedistPath=$VCRedistPath",
    "/DOutputDir=$outputPath",
    $installerScript
)

& $InnoCompilerPath @compilerArguments
if ($LASTEXITCODE -ne 0) {
    throw "Inno Setup failed to compile the wxChaos installer."
}

$appVersion = (Get-Item -LiteralPath $stagedExecutable).VersionInfo.ProductVersion
$installerPath = Join-Path $outputPath "wxChaos-$appVersion-windows-x64-setup.exe"
if (-not (Test-Path -LiteralPath $installerPath -PathType Leaf)) {
    throw "The expected installer artifact was not created: $installerPath"
}

Write-Output $installerPath
