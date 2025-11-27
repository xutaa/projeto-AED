#!/usr/bin/env bash
# Extract ImageIsEqual deep_equal comparison data
set -euo pipefail

input="isEqualResults.csv"
output="imageIsEqual_analysis.csv"

# Write header
echo "Size,Pixels,PixComp,PixMem" > "$output"

# Extract deep_equal rows for chess pattern, extract relevant fields
grep "ImageIsEqual,deep_equal,chess" "$input" | \
    awk -F, '{
        # Extract dimensions
        width = $5
        height = $6
        pixels = $7
        pixmem = $11
        pixcomp = $15
        
        # Format size
        if (width == height) {
            size = width
        } else {
            size = width "x" height
        }
        
        # Print CSV row
        printf "%s,%s,%s,%s\n", size, pixels, pixcomp, pixmem
    }' >> "$output"

echo "Created $output with ImageIsEqual analysis data"
cat "$output"
