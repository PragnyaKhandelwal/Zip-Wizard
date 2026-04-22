param(
    [string]$Keyword = "gamma"
)

$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $PSScriptRoot
Set-Location $root

if (-not (Test-Path ".\\benchmark_data")) {
    & ".\scripts\generate-benchmark-dataset.ps1"
}

& ".\build.ps1" | Out-Null

Push-Location ".\benchmark_data"
try {
    $stdinFile = [System.IO.Path]::GetTempFileName()
    $stdoutFile = [System.IO.Path]::GetTempFileName()
    try {
        [System.IO.File]::WriteAllText($stdinFile, "5`nk`n$Keyword`n9`n")
        $proc = Start-Process -FilePath "..\zipwizard.exe" -RedirectStandardInput $stdinFile -RedirectStandardOutput $stdoutFile -NoNewWindow -PassThru -Wait
        if ($proc.ExitCode -ne 0) {
            throw "Search benchmark process failed with exit code $($proc.ExitCode)."
        }
        $result = Get-Content $stdoutFile
    } finally {
        Remove-Item $stdinFile, $stdoutFile -Force -ErrorAction SilentlyContinue
    }
} finally {
    Pop-Location
}

Write-Host "---- Timing Output ----"
$result | Select-String "\[Timing\]" | ForEach-Object { $_.Line }

if (-not ($result | Select-String "\[Timing\] Search File completed")) {
    throw "Search benchmark did not produce timing output."
}

Write-Host "Search benchmark run complete."
