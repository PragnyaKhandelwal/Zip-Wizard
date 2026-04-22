param(
    [string]$Compiler = "gcc",
    [string]$Output = "zipwizard.exe"
)

$ErrorActionPreference = "Stop"

$projectRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
Set-Location $projectRoot

$sourceFiles = Get-ChildItem -Path ".\src\*.c" | ForEach-Object { $_.FullName }
if (-not $sourceFiles -or $sourceFiles.Count -eq 0) {
    throw "No source files found under src/."
}

Write-Host "Building Zip Wizard..."
& $Compiler @sourceFiles "-I" ".\include" "-Wall" "-Wextra" "-O2" "-o" $Output

if ($LASTEXITCODE -ne 0) {
    throw "Build failed with exit code $LASTEXITCODE"
}

Write-Host "Build successful: $Output"
