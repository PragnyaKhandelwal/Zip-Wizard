$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $PSScriptRoot
Set-Location $root

function Extract-Number {
    param(
        [string]$Text,
        [string]$Pattern,
        [double]$DefaultValue = 0.0
    )

    $match = [regex]::Match($Text, $Pattern)
    if ($match.Success) {
        return [double]$match.Groups[1].Value
    }
    return $DefaultValue
}

function New-Bar {
    param(
        [double]$Value,
        [double]$Max,
        [int]$Width = 28
    )

    if ($Max -le 0) {
        return ""
    }

    $count = [Math]::Max(1, [Math]::Round(($Value / $Max) * $Width))
    return ("#" * [int]$count)
}

# Ensure data and baseline build exist
$null = & ".\scripts\generate-benchmark-dataset.ps1" 2>&1 | Out-String

$searchOutput = (& ".\scripts\run-search-benchmark.ps1" 2>&1 | Out-String)
$compressionOutput = (& ".\scripts\run-compression-benchmark.ps1" 2>&1 | Out-String)
$indexOutput = (& ".\scripts\run-index-benchmark.ps1" 2>&1 | Out-String)

$searchMs = Extract-Number -Text $searchOutput -Pattern "Search File completed in ([0-9.]+) ms"

$naiveCompressionMs = Extract-Number -Text $compressionOutput -Pattern "Naive\s+: time=([0-9.]+) ms"
$optimizedCompressionMs = Extract-Number -Text $compressionOutput -Pattern "HashChain\s+: time=([0-9.]+) ms"
$compressionSpeedup = Extract-Number -Text $compressionOutput -Pattern "Speedup \(Naive/HashChain\): ([0-9.]+)x"

$indexLookupMs = Extract-Number -Text $indexOutput -Pattern "Hash index lookup time: ([0-9.]+) ms"
$findFirstFileMs = Extract-Number -Text $indexOutput -Pattern "FindFirstFile lookup time: ([0-9.]+) ms"
$indexSpeedup = Extract-Number -Text $indexOutput -Pattern "Speedup \(FindFirstFile/Index\): ([0-9.]+)x"

$maxCompression = [Math]::Max($naiveCompressionMs, $optimizedCompressionMs)
$maxIndex = [Math]::Max($findFirstFileMs, $indexLookupMs)

$reportPath = ".\docs\BENCHMARK_REPORT.md"
$generatedAt = Get-Date -Format "yyyy-MM-dd HH:mm:ss"

$report = @"
# Zip Wizard CP/PBL Milestone 5 Report

Generated at: $generatedAt

## Summary

- Milestone 1: Timing instrumentation and benchmark datasets implemented.
- Milestone 2: KMP-based keyword search implemented.
- Milestone 3: Hash-chain LZ77 matching implemented and benchmarked.
- Milestone 4: Filename hash index cache implemented and benchmarked.
- Milestone 5: Complexity table and benchmark report published.

## Benchmark Results

### Search (KMP Path)

- Search operation time (menu benchmark run): **$([string]::Format("{0:N3}", $searchMs)) ms**

### Compression Benchmark (Naive vs HashChain)

- Naive compression time: **$([string]::Format("{0:N3}", $naiveCompressionMs)) ms**
- HashChain compression time: **$([string]::Format("{0:N3}", $optimizedCompressionMs)) ms**
- Compression speedup: **$([string]::Format("{0:N3}", $compressionSpeedup))x**

### Index Benchmark (FindFirstFile vs Hash Index)

- Hash index lookup time: **$([string]::Format("{0:N3}", $indexLookupMs)) ms**
- FindFirstFile lookup time: **$([string]::Format("{0:N3}", $findFirstFileMs)) ms**
- Index speedup: **$([string]::Format("{0:N3}", $indexSpeedup))x**

## Complexity Table (Before vs After)

| Operation | Before | After | Notes |
|---|---|---|---|
| Exact filename lookup | O(N) filesystem scan | O(1) average (hash index) | Cache synced on create/rename/delete |
| Keyword search in a file | O(n*m) worst-case naive substring checks | O(n + m) with KMP | Streaming KMP implementation |
| LZ77 match search | Near-quadratic practical behavior with naive window scan | Near-linear average practical behavior with rolling-hash + hash-chain | Bounded chain traversal |
| Menu operation timing | Not measured | Measured per operation | Printed in CLI for benchmark evidence |

## Performance Graphs (Text)

### Compression Time (lower is better)

- Naive     [$([string]::Format("{0,8:N3}", $naiveCompressionMs)) ms]  $(New-Bar -Value $naiveCompressionMs -Max $maxCompression)
- HashChain [$([string]::Format("{0,8:N3}", $optimizedCompressionMs)) ms]  $(New-Bar -Value $optimizedCompressionMs -Max $maxCompression)

### Lookup Time (lower is better)

- FindFirstFile [$([string]::Format("{0,8:N3}", $findFirstFileMs)) ms]  $(New-Bar -Value $findFirstFileMs -Max $maxIndex)
- Hash Index    [$([string]::Format("{0,8:N3}", $indexLookupMs)) ms]  $(New-Bar -Value $indexLookupMs -Max $maxIndex)

## Reproducibility

Run these commands from repository root:

``````powershell
./scripts/generate-benchmark-dataset.ps1
./scripts/run-search-benchmark.ps1
./scripts/run-compression-benchmark.ps1
./scripts/run-index-benchmark.ps1
./scripts/generate-milestone5-report.ps1
``````

## Raw Captured Outputs

### Search Benchmark Output

``````text
$searchOutput
``````

### Compression Benchmark Output

``````text
$compressionOutput
``````

### Index Benchmark Output

``````text
$indexOutput
``````
"@

Set-Content -Path $reportPath -Value $report -Encoding UTF8
Write-Host "Milestone 5 report generated at $reportPath"
