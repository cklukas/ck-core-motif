#!/usr/bin/env python3
"""Check ProgressRing cleans up partial GC initialization."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/ProgressRing.c").read_text(
    encoding="utf-8"
)
start = source.index("static void Initialize(Widget request")
end = source.index("static void Destroy", start)
context = source[start:end]
required = (
    "if (!ring->progress_ring.track_gc || !ring->progress_ring.fill_gc ||",
    "XFreeGC(display, ring->progress_ring.track_gc);",
    "ring->progress_ring.text_gc = NULL;",
)
missing = [fragment for fragment in required if fragment not in context]
if missing:
    raise SystemExit("ProgressRing GC setup cleanup missing: " + ", ".join(missing))

print("ProgressRing GC setup check passed")
