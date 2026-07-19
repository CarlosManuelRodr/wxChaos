$installerScript = Join-Path $PSScriptRoot "..\install\InnoSetup\BuildInstaller.ps1"

& $installerScript @args
