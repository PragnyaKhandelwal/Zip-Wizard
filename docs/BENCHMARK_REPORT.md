# Zip Wizard CP/PBL Milestone 5 Report

Generated at: 2026-04-22 23:21:53

## Summary

- Milestone 1: Timing instrumentation and benchmark datasets implemented.
- Milestone 2: KMP-based keyword search implemented.
- Milestone 3: Hash-chain LZ77 matching implemented and benchmarked.
- Milestone 4: Filename hash index cache implemented and benchmarked.
- Milestone 5: Complexity table and benchmark report published.

## Benchmark Results

### Search (KMP Path)

- Search operation time (menu benchmark run): **3.286 ms**

### Compression Benchmark (Naive vs HashChain)

- Naive compression time: **404.226 ms**
- HashChain compression time: **46.663 ms**
- Compression speedup: **8.663x**

### Index Benchmark (FindFirstFile vs Hash Index)

- Hash index lookup time: **0.103 ms**
- FindFirstFile lookup time: **342.647 ms**
- Index speedup: **3,336.392x**

## Complexity Table (Before vs After)

| Operation | Before | After | Notes |
|---|---|---|---|
| Exact filename lookup | O(N) filesystem scan | O(1) average (hash index) | Cache synced on create/rename/delete |
| Keyword search in a file | O(n*m) worst-case naive substring checks | O(n + m) with KMP | Streaming KMP implementation |
| LZ77 match search | Near-quadratic practical behavior with naive window scan | Near-linear average practical behavior with rolling-hash + hash-chain | Bounded chain traversal |
| Menu operation timing | Not measured | Measured per operation | Printed in CLI for benchmark evidence |

## Performance Graphs (Text)

### Compression Time (lower is better)

- Naive     [ 404.226 ms]  ############################
- HashChain [  46.663 ms]  ###

### Lookup Time (lower is better)

- FindFirstFile [ 342.647 ms]  ############################
- Hash Index    [   0.103 ms]  #

## Reproducibility

Run these commands from repository root:

```powershell
./scripts/generate-benchmark-dataset.ps1
./scripts/run-search-benchmark.ps1
./scripts/run-compression-benchmark.ps1
./scripts/run-index-benchmark.ps1
./scripts/generate-milestone5-report.ps1
```

## Raw Captured Outputs

### Search Benchmark Output

```text
[Timing] Search File completed in 3.286 ms

```

### Compression Benchmark Output

```text
Compression Benchmark: .\benchmark_data\large.txt
- Naive     : time=404.226 ms, tokens=277024
- HashChain : time=46.663 ms, tokens=369026
- Speedup (Naive/HashChain): 8.663x

```

### Index Benchmark Output

```text
File Index Benchmark: name=large.txt iterations=10000
- Hash index lookup time: 0.103 ms
- FindFirstFile lookup time: 342.647 ms
- Speedup (FindFirstFile/Index): 3336.392x

```
