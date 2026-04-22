param(
    [string]$Binary = "zipwizard.exe"
)

$ErrorActionPreference = "Stop"
$projectRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
Set-Location $projectRoot

if (-not (Test-Path ".\\$Binary")) {
    Write-Host "Binary not found. Building first..."
    & ".\build.ps1" -Output $Binary
}

& ".\\$Binary"
