param(
    [string]$OutputDir = "benchmark_data",
    [int]$SmallLines = 500,
    [int]$MediumLines = 5000,
    [int]$LargeLines = 25000
)

$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $PSScriptRoot
Set-Location $root

if (-not (Test-Path ".\\$OutputDir")) {
    New-Item -ItemType Directory -Path ".\\$OutputDir" | Out-Null
}

function New-DatasetFile {
    param(
        [string]$Path,
        [int]$LineCount,
        [string]$Needle
    )

    $writer = [System.IO.StreamWriter]::new($Path, $false)
    try {
        for ($i = 1; $i -le $LineCount; $i++) {
            if ($i % 997 -eq 0) {
                $writer.WriteLine("line=$i payload=zipwizard benchmark keyword=$Needle")
            } else {
                $token = [guid]::NewGuid().ToString('N').Substring(0, 12)
                $writer.WriteLine("line=$i payload=zipwizard benchmark randomtoken=$token")
            }
        }
    } finally {
        $writer.Dispose()
    }
}

New-DatasetFile -Path ".\\$OutputDir\\small.txt" -LineCount $SmallLines -Needle "alpha"
New-DatasetFile -Path ".\\$OutputDir\\medium.txt" -LineCount $MediumLines -Needle "beta"
New-DatasetFile -Path ".\\$OutputDir\\large.txt" -LineCount $LargeLines -Needle "gamma"

Write-Host "Benchmark datasets generated under ./$OutputDir"
