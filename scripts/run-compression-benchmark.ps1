param(
    [string]$InputFile = ".\benchmark_data\large.txt"
)

$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $PSScriptRoot
Set-Location $root

if (-not (Test-Path ".\benchmark_data")) {
    & ".\scripts\generate-benchmark-dataset.ps1"
}

if (-not (Test-Path $InputFile)) {
    throw "Input file not found: $InputFile"
}

& ".\build.ps1" | Out-Null

Write-Host "Running compression benchmark on $InputFile"
& .\zipwizard.exe --benchmark-compress $InputFile
