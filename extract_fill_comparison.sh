#!/usr/bin/env bash
# Extract flood filling comparison data across different strategies and image types
set -euo pipefail

input="results_sweep.csv"
output="fill_comparison.csv"

# Write header
echo "Strategy,ImageType,Size,Pixels,Time_ms,PixValidations,StackOps,QueueOps,PeakStack,PeakQueue,PeakRecDepth" > "$output"

# Extract fill rows and process
grep "^fill," "$input" | awk -F, '{
    strategy = $2
    img = $3
    width = $5
    height = $6
    pixels = $7
    painted = $8
    time_sec = $9
    time_ms = time_sec * 1000
    pixval = $15
    stackops = $16
    queueops = $17
    peakstack = $18
    peakqueue = $19
    peakrec = $20
    
    # Determine image type
    if (img ~ /^white/) {
        imgtype = "white"
    } else if (img ~ /^chess/) {
        imgtype = "chess"
    } else if (img ~ /^maze/) {
        imgtype = "maze"
    } else {
        imgtype = "other"
    }
    
    # Format size
    size = width "x" height
    
    # Only include if pixels were actually painted
    if (painted > 0) {
        printf "%s,%s,%s,%s,%.3f,%s,%s,%s,%s,%s,%s\n", 
            strategy, imgtype, size, pixels, time_ms, pixval, stackops, queueops, peakstack, peakqueue, peakrec
    }
}' >> "$output"

echo "Created $output"
echo ""
echo "Sample data by image type:"
echo ""
echo "=== White (uniform) images ==="
grep ",white," "$output" | head -6
echo ""
echo "=== Chess (segmented) images ==="
grep ",chess," "$output" | head -6
echo ""
echo "=== Maze images ==="
grep ",maze," "$output" | head -9
