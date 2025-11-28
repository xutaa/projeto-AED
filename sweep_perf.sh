#!/usr/bin/env bash
# Sweep image sizes and collect performance CSV
# Usage: ./sweep_perf.sh
set -euo pipefail
start_ts=$(date +%s%N)

out="results_sweep.csv"

# Build latest perf_test
make -s perf_test

# Increase stack size to avoid segfault with deep recursion on large images
ulimit -s 65536

# Run perf_test (uses built-in default sizes + maze)
./perf_test > "$out"

end_ts=$(date +%s%N)
elapsed_ns=$((end_ts - start_ts))
# Convert to seconds with 3 decimal places using awk for portability
elapsed_sec=$(awk -v ns="$elapsed_ns" 'BEGIN { printf "%.3f", ns/1000000000 }')

echo "Written $out with default sizes and maze"
echo "Total sweep runtime: ${elapsed_sec}s"
make clean >/dev/null