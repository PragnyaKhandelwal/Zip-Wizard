# ZIP WIZARD - File Handling and Compression System

## Project Overview

**ZipWizard** is a Windows CLI tool for file operations and basic compression/decompression using an LZ77-style approach. It supports create, edit, rename, delete, search, file info, zip, and unzip operations from an interactive menu.

## Reorganized Project Structure

The codebase is now physically organized for maintainability:

```
Zip-Wizard/
  README.md
  build.ps1
  run.ps1
  smoke-test.ps1
  .gitignore
  include/
    *.h
  src/
    *.c
  docs/
    CP_OPTIMIZATION_ROADMAP.md
  scripts/
    generate-benchmark-dataset.ps1
    run-search-benchmark.ps1
    run-compression-benchmark.ps1
    run-index-benchmark.ps1
    generate-milestone5-report.ps1
```

## Build and Run

Open PowerShell in repository root and run:

```powershell
./build.ps1
./run.ps1
```

You can also run a smoke test:

```powershell
./smoke-test.ps1
```

## Feature Set

- File operations: create, edit, rename, delete.
- Search: by file name pattern or by keyword in `.txt` files.
- File metadata: full path, attributes, and timestamps.
- Compression/decompression: LZ77-based zip/unzip workflow.
- CLI UX: menu-driven interaction with input validation.

## CP/PBL Alignment

This project now has a dedicated optimization roadmap for Competitive Programming / PBL goals:

- Complexity analysis baseline
- Data structure upgrades (hash map index, inverted index)
- Algorithm upgrades (KMP/Aho-Corasick, improved LZ77 matching)
- Benchmark-driven optimization process

See `./docs/CP_OPTIMIZATION_ROADMAP.md` for details.

## Milestone 1 and 2 Status

- Milestone 1 implemented:
  - Timing instrumentation now measures every menu operation and prints elapsed time in milliseconds.
  - Benchmark dataset generator script added: `./scripts/generate-benchmark-dataset.ps1`.
  - Search benchmark runner added: `./scripts/run-search-benchmark.ps1`.

- Milestone 2 implemented:
  - Keyword search now uses Knuth-Morris-Pratt (KMP) matching over file streams instead of `strstr` per line.
  - This improves theoretical matching complexity from repeated naive checks to `O(n + m)` per file.

## Milestone 3 Status

- Implemented rolling-hash + hash-chain-assisted LZ77 matching in compression path.
- Added benchmark mode to compare naive vs optimized compression:

```powershell
./zipwizard.exe --benchmark-compress .\benchmark_data\large.txt
```

- Added convenience runner:

```powershell
./scripts/run-compression-benchmark.ps1
```

- The benchmark prints naive time, optimized time, and computed speedup.

## Milestone 4 Status

- Added in-memory hash-map style file index cache loaded at startup.
- CRUD flows now keep index synchronized:
  - Create adds filename to index.
  - Delete removes filename from index.
  - Rename updates old/new mapping in index.
- Exact name search now uses index lookup (`O(1)` average), while wildcard search keeps filesystem scan behavior.
- Added index benchmark mode:

```powershell
./zipwizard.exe --benchmark-index large.txt 10000
./scripts/run-index-benchmark.ps1
```

## Milestone 5 Status

- Added automated report generation script that runs benchmarks and publishes a CP/PBL-ready report:

```powershell
./scripts/generate-milestone5-report.ps1
```

- Report output:
  - `./docs/BENCHMARK_REPORT.md`

- Includes:
  - Before/after complexity table
  - Search/compression/index benchmark metrics
  - Text-based performance graphs
  - Raw benchmark outputs for reproducibility

## Recommended Next Enhancements

1. Add benchmark mode (`--benchmark`) with timing output for each operation.
2. Add non-interactive CLI flags (`--zip`, `--unzip`, `--search`) for automation.
3. Introduce index caching to speed up repeated searches.
4. Upgrade compression matching with rolling hash for better performance.
5. Add regression tests for stable behavior under larger inputs.
