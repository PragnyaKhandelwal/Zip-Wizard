# CP/PBL Optimization Roadmap for Zip Wizard

## Current Complexity Snapshot

- Create/Edit/Delete/Rename by exact path: `O(1)` average (filesystem dependent).
- Search by name with wildcard (`FindFirstFile`): `O(N)` over directory entries.
- Search by keyword in all txt files: `O(total_bytes_scanned)`.
- Compression/decompression: linear to input size, currently with high constants due to naive matching.

## High-Value DS/Algo Upgrades

1. Add a file index cache
- DS: hash map (`filename -> metadata`) loaded at startup and updated on write ops.
- Benefit: common name lookups become `O(1)` average after first scan.

2. Fast keyword search engine
- DS/Algo options:
  - KMP per file for single-pattern searches.
  - Aho-Corasick for multi-keyword queries in one pass.
  - Inverted index (`token -> list<file>`) for repeated interactive queries.
- Benefit: lower repeated query time in CLI sessions.

3. Better compression backend
- Replace current naive LZ77 match scan with rolling hash + hash chains.
- Keep sliding window but index substrings by hash buckets.
- Benefit: practical speedup from near-quadratic matching behavior to close-to-linear average.

4. Add operation scheduler for batch mode
- DS: queue of file operations and bounded worker pool for IO-heavy tasks.
- Benefit: scalable CLI for batch automation and stress tests.

5. Add benchmark harness
- Measure throughput for search/compress/decompress across multiple file sizes.
- Track metrics: MB/s, latency percentiles, memory footprint.

## Engineering Improvements for Scalability

1. Introduce modular layers
- `core/` for business logic
- `cli/` for user interaction
- `platform/` for Windows-specific wrappers

2. Add consistent error model
- Return status enums and central error-to-message mapping.

3. Add regression tests
- Unit tests for parsers and helpers.
- Integration tests for full menu flows with scripted input.

4. Add batch mode CLI flags
- Example: `zipwizard.exe --zip file.txt --out file.zip`
- This keeps menu mode for demo and adds automation for CP-style experiments.

## Suggested CP/PBL Milestones

1. Milestone 1
- Build reproducible benchmark datasets.
- Add timing instrumentation around every operation.

2. Milestone 2
- Implement KMP search path.
- Compare against baseline line-scan search.

3. Milestone 3
- Implement rolling-hash-assisted LZ77 matching. (Implemented)
- Compare compression time and ratio. (Implemented via `zipwizard.exe --benchmark-compress <file>`)

4. Milestone 4
- Add index cache and benchmark repeated query sessions. (Implemented)

5. Milestone 5
- Publish complexity table and benchmark graphs in report. (Implemented via `docs/BENCHMARK_REPORT.md`)

## Target Outcome for CP/PBL

- Clear before/after algorithmic complexity story.
- Quantified runtime gains with reproducible benchmarks.
- Demonstrated data-structure-driven design decisions.
