#!/usr/bin/env python3
"""Check that ScrollFrame navigator-list growth cannot wrap."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "ScrollFrameT.c"
text = source.read_text(encoding="utf-8")
for needle in (
    "num_nav_slots > (Cardinal)-1 - 2",
    "SIZE_MAX / sizeof(Widget)",
    "(size_t)(Cardinal)-1 / sizeof(Widget)",
):
    if needle not in text:
        raise SystemExit(f"navigator-list growth guard is missing: {needle}")

print("ScrollFrame navigator-list growth guards present")
