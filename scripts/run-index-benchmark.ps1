param(
    [string]$FileName = "large.txt",
    [int]$Iterations = 10000
)

$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $PSScriptRoot
Set-Location $root

if (-not (Test-Path ".\benchmark_data")) {
    & ".\scripts\generate-benchmark-dataset.ps1"
}

& ".\build.ps1" | Out-Null

Push-Location ".\benchmark_data"
try {
    & ..\zipwizard.exe --benchmark-index $FileName $Iterations
} finally {
    Pop-Location
}
