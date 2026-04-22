$ErrorActionPreference = "Stop"
$projectRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
Set-Location $projectRoot

# Build fresh binary
if (Test-Path ".\zipwizard.exe") {
    Remove-Item ".\zipwizard.exe" -Force
}
& ".\build.ps1" -Output "zipwizard.exe"

# Prepare deterministic smoke input flow
$input = @"
1
cp_smoke.txt
hello cp pbl
7
cp_smoke.txt
8
cp_smoke.txt.zip
3
cp_smoke.txt
cp_smoke_renamed.txt
5
n
cp_smoke_renamed.txt
6
cp_smoke_renamed.txt
4
cp_smoke_renamed.txt
9
"@

$input | .\zipwizard.exe | Out-Null

# Validate expected artifacts from zip/unzip
if (-not (Test-Path ".\cp_smoke.txt.zip")) {
    throw "Smoke test failed: expected cp_smoke.txt.zip not found."
}
if (-not (Test-Path ".\cp_smoke.txt_output.txt")) {
    throw "Smoke test failed: expected cp_smoke.txt_output.txt not found."
}

# Cleanup test files
Remove-Item ".\cp_smoke.txt.zip", ".\cp_smoke.txt_output.txt", ".\zipwizard.exe" -Force -ErrorAction SilentlyContinue

Write-Host "Smoke test passed."
